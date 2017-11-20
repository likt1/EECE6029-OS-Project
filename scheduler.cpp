#include "scheduler.h"
#include "jobs.h"

#include "stdio.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>

scheduler::scheduler()  {}

scheduler::~scheduler() { 
	clearScheduler(); 
}

//====================== Push functions ======================//
// Add job to end of list
// The list manages all pointers assigned to it. Do not delete pointers assigned
//   to any part of the list.

void scheduler::push(job* job, int ct) {
  result newResult;
  newResult.completedJob   = job;
  newResult.completionTime = ct;
  newResult.turnAroundTime = ct - (job->arrivalTime);
  newResult.waitingTime    = (newResult.turnAroundTime) - (job->burstTime);
  
  this->resultsList.push_back(newResult);
}

void scheduler::pushHistory(job* job, int startTime, int burst) {
  historyObj newHistory;
  newHistory.orgJbPntr  = job;
  newHistory.stTime     = startTime;
  newHistory.burst      = burst;

  this->history.push_back(newHistory);
}

//====================== Access and count functions ======================//
result* scheduler::getAt(int index) {
  if (index < this->resultsList.size()) {
    return &(this->resultsList[index]);
  }
  
  return NULL;
}

int scheduler::size() {
  return this->resultsList.size();
}

int scheduler::numSchedulers() {
  return 5;
}

//======================== SCHEDULERS ========================//
void scheduler::FIFO(jobs* schedulerJobs) {
	this->clearScheduler();
  schedulerJobs->sort(2);

  int currentTime = 0;

  for (int i = 0; i < schedulerJobs->size(); i++) {

    job* nextJob = schedulerJobs->getAt(i);

    // if there are no jobs pending at current time, 
    // we advance current time to the next job
    if (currentTime < nextJob->arrivalTime) {
    	currentTime = nextJob->arrivalTime;
    } 

    currentTime += nextJob->burstTime;

    this->push(nextJob, currentTime);
    this->pushHistory(nextJob, currentTime - nextJob->burstTime, nextJob->burstTime);

    }
}

void scheduler::idealSJF(jobs* schedulerJobs) {
  this->clearScheduler();
  schedulerJobs->sort(3); //sort by burst

  int currentTime = 0;
  int jobsRemaining = schedulerJobs->size();
  std::vector<bool> jobCompleted(jobsRemaining, false);

  // This is not a particularly efficient algorithm, I know.
  // But on the scale of scheduling that we're doing in the alpha
  // I don't see it being an issue, do you?
  while (jobsRemaining != 0) {
    int incrementCurrentTime = 1;

    // check all the jobs before or at current time.
    // pick the first incomplete one.
    // because they're sorted by BT, the first will be shortest
    for (int i=0; i < schedulerJobs->size(); i++) {
      job* currentJob = schedulerJobs->getAt(i);
      if ((currentJob->arrivalTime <= currentTime) && (!jobCompleted[i])) {
        // found the shortest incomplete job
        // do the job
        incrementCurrentTime = currentJob->burstTime;
        this->push(currentJob, currentTime+incrementCurrentTime);
        this->pushHistory(currentJob, currentTime, currentJob->burstTime);

        //mark it as complete
        jobCompleted[i] = true;
        --jobsRemaining;
        break;
      }
    }

    currentTime += incrementCurrentTime;
  }
}

// Predicts next burst of a process with exponential aging
// where an average of previous bursts for a process are taken 
// and the weight of the average decays wtih time
void scheduler::realSJF(jobs* schedulerJobs, double alpha, int initialBT) {
  std::map <int, int> predictionMap;

  this->clearScheduler();
  schedulerJobs->sort(2); //sort by time

  int currentTime = 0;
  int jobsRemaining = schedulerJobs->size();
  std::vector<bool> jobCompleted(jobsRemaining, false);

  while (jobsRemaining != 0) {
    int incrementCurrentTime = 1;

    int shortestJobBT  = -1;
    int shortestJobIdx = -1;

    for (int i=0; i < schedulerJobs->size(); i++) {
      if (jobCompleted[i]) { continue; }
      
      job* currentJob = schedulerJobs->getAt(i);
      if (currentJob->arrivalTime <= currentTime) {
        // create an entry for the job in our map if it doesn't exist yet
        if (predictionMap.count(currentJob->processNum) == 0) {
          predictionMap[currentJob->processNum] = initialBT;
          // printf("Creating new entry for P# %i\n", currentJob->processNum);
        }

        // if this is the shortest job or first job we're inspecting, it's the new shortest job
        if ((shortestJobBT == -1) || 
             predictionMap[currentJob->processNum] < shortestJobBT) {

          // printf("P#%i has a predicted BT of %i, which is better than %i.\n", currentJob->processNum, predictionMap[currentJob->processNum], shortestJobBT);
          shortestJobBT  = predictionMap[currentJob->processNum];
          shortestJobIdx = i;
        }
      }
    }

    if (shortestJobIdx != -1) {
      // run the job & mark it as complete
      job* jobToDo = schedulerJobs->getAt(shortestJobIdx);
      incrementCurrentTime = jobToDo->burstTime;
      this->push(jobToDo, currentTime+incrementCurrentTime);
      this->pushHistory(jobToDo, currentTime, jobToDo->burstTime);

      jobCompleted[shortestJobIdx] = true;
      --jobsRemaining;

      // update estimate for next run
      // predictedBT = alpha*actualPreviousBT + (1 - alpha)*predictedPreviousBT;
      // printf("Job %i. Prediction: %i, Actual: %i, ", jobToDo->processNum, predictionMap[jobToDo->processNum], jobToDo->burstTime);
      predictionMap[jobToDo->processNum] = alpha * jobToDo->burstTime + (1 - alpha) * predictionMap[jobToDo->processNum];
      // printf("New Prediction: %i\n", predictionMap[jobToDo->processNum]);
    }

    currentTime += incrementCurrentTime;
  }
}

// addToJobQueue is shared between RR and TB-PQ
void scheduler::addToJobQueue(jobs* sj, int tj, int* rj, int ct, queueObjList* q) {
  bool complete = false; // make sure all jobs that arrive at the current time are processed
  while ((*rj) > 0 && !complete) {
    job* nextReadyJob = sj->getAt(tj - (*rj)); // get next ready job
    
    // if that job arrived at the current time
    if (nextReadyJob->arrivalTime == ct) { 
      queueObj temp; // construct queueJob
      temp.originalJobPointer = nextReadyJob;
      temp.queueJob = *nextReadyJob;
    
      q->push_back(temp); // create a copy and insert into queue
      (*rj)--;
    }
    else {
      complete = true;
    }
  }
}

// simulated scheduler works in 4 steps that are repeated until we complete all 
//  jobs.
//    1. Assign cpu to work on a job
//    2. Work on the job and handle whatever happens for that job
//    3. Add incoming jobs to the internal job queue
//    4. End loop of all jobs are finished
void scheduler::roundRobin(jobs* schedulerJobs) {
  this->clearScheduler();
  //schedulerJobs->sort(2); // sort on arrival time

  int currentTime = 0; // system clock
  int totalJobs = schedulerJobs->size();
  int remainingJobs = totalJobs;
  int timeQuantum = 5; // time to take on job before preemption
  int timeWorked = 0;
  bool running = true;
  bool isWorking = false;
  queueObjList queue; // std::list<queueObj>
  queueObjList::iterator working; // which job we are currently working on

  while (running) {
    // if we aren't working on a job and there are jobs in queue, 
    //  1. begin work on next job
    if (!isWorking && !queue.empty()) {
      // begin() returns iterator, which is basically a pointer to the element
      working = queue.begin();
      isWorking = true;
    }
    
    // 2. work on job if we are working on a job.
    if (isWorking) {
      (*working).queueJob.burstTime--;
      timeWorked++;
      
      // if we complete a job then add to results
      if ((*working).queueJob.burstTime == 0) {
        this->push((*working).originalJobPointer, currentTime);
        this->pushHistory((*working).originalJobPointer, currentTime - timeWorked, 
                          timeWorked);
        queue.erase(working);
        isWorking = false;
        timeWorked = 0;
      }
      
      // if we didn't complete it, but the time quantum is reached, preempt
      if (timeWorked == timeQuantum) {
        this->pushHistory((*working).originalJobPointer, currentTime - timeWorked, 
                          timeWorked);
        queue.splice(queue.end(), queue, working);
        isWorking = false;
        timeWorked = 0;
      }
    }
    
    // 3. if next ready job has arrived given the current time, add to queue
    this->addToJobQueue(schedulerJobs, totalJobs, &remainingJobs, currentTime, &queue);
    
    // 4. if all jobs are finished
    if (!isWorking && queue.empty() && remainingJobs == 0) {
      running = false;
    }
    
    currentTime++; // inc time
  }
}

// supporting functions for age based priority
// Calculate priority given the amount of time the job has been waiting
int scheduler::calcPri(int currentTime, job* job, int minWait) {
  int updatedPri = job->priority - (currentTime - job->arrivalTime)/minWait;
  if (updatedPri < 0) {
    return 0;
  }
  return updatedPri;
}

// Non-preemptive priority scheduler based on age
void scheduler::ageBasedPri(jobs* schedulerJobs) {
	this->clearScheduler();
  //schedulerJobs->sort(2); // sort on arrival time

  int currentTime = 0; // system clock
  int totalJobs = schedulerJobs->size();
  int remainingJobs = totalJobs;
  bool running = true;
  bool isWorking = false;
  queueObjList queue; // std::list<queueObj>
  queueObjList::iterator working; // which job we are currently working on

  while (running) {
    // if we aren't working on a job and there are jobs in queue, 
    //  1. find highest priority job in queue and begin work on it
    if (!isWorking && !queue.empty()) {
      // begin() returns iterator, which is basically a pointer to the element
      queueObjList::iterator highest = queue.begin();
      queueObjList::iterator next = queue.begin();
      ++next;
      for (; next != queue.end(); ++next) {
        job* hJob = &((*highest).queueJob);
        job* itJob = &((*next).queueJob);
        
        int hPri = this->calcPri(currentTime, hJob, 20);
        int itPri = this->calcPri(currentTime, itJob, 20);
      
        if (hPri > itPri || 
            (hPri == itPri && 
            hJob->arrivalTime > itJob->arrivalTime) ||
            (hPri == itPri && 
            hJob->arrivalTime == itJob->arrivalTime &&
            hJob->processNum > itJob->processNum)) {
          highest = next;
        }
      }
      working = highest;
      isWorking = true;
    }
    
    // 2. work on job if we are working on a job (non-preemptive)
    if (isWorking) {
      (*working).queueJob.burstTime--;
      
      // if we complete a job then add to results
      if ((*working).queueJob.burstTime == 0) {
        this->push((*working).originalJobPointer, currentTime);
        this->pushHistory((*working).originalJobPointer, 
                          currentTime - (*working).originalJobPointer->burstTime, 
                          (*working).originalJobPointer->burstTime);
        queue.erase(working);
        isWorking = false;
      }
    }
    
    // 3. if next ready job has arrived given the current time, add to queue
    this->addToJobQueue(schedulerJobs, totalJobs, &remainingJobs, currentTime, &queue);
    
    // 4. if all jobs are finished
    if (!isWorking && queue.empty() && remainingJobs == 0) {
      running = false;
    }
    
    currentTime++;
  }
}

// Multi level feedback queue
void scheduler::MLFQ(jobs* schedulerJobs) {
  this->clearScheduler();
  //schedulerJobs->sort(2); // sort on arrival time

  int numQueues = 3;
  int currentTime = 0; // system clock
  int totalJobs = schedulerJobs->size();
  int remainingJobs = totalJobs;
  int timeQuantum[] = {15, 30, 50}; // time to take on job before preemption for each queue
  bool running = true;
  
  queueObjList queues[numQueues]; // 3 levels, std::list<queueObj>
  queueObjList::iterator working; // which job we are currently working on
  bool isWorking = false;
  int timeWorked = 0;
  int workingQueue = 0;

  while (running) {
    // Make sure we are working on the highest queue job and preempt if we aren't
    //  1. begin work on highest queue job
    bool found = false;
    int queueLim = numQueues; // Search all the queues...
    if (isWorking) { // ...only if we aren't working
      queueLim = workingQueue;
    }
    for (int i = 0; i < queueLim && !found; i++) {
      if (!queues[i].empty()) {
        if (isWorking) { // preempt if we are working
          queues[workingQueue].splice(queues[workingQueue].end(), queues[workingQueue], working);
        }
        // begin() returns iterator, which is basically a pointer to the element
        working = queues[i].begin();
        workingQueue = i;
        isWorking = true;
        timeWorked = 0; // reset time worked
        found = true; // stop searching
      }
    }
    
    // 2. work on job if we are working on a job.
    if (isWorking) {
      (*working).queueJob.burstTime--;
      timeWorked++;
      
      // if we complete a job then add to results
      if ((*working).queueJob.burstTime == 0) {
        this->push((*working).originalJobPointer, currentTime);
        this->pushHistory((*working).originalJobPointer, currentTime - timeWorked, 
                          timeWorked);
        queues[workingQueue].erase(working);
        isWorking = false;
        timeWorked = 0; // reset time worked
      }
      
      // if we didn't complete it, but the time quantum is reached, preempt to the lower queue
      if (timeWorked == timeQuantum[workingQueue]) {
        this->pushHistory((*working).originalJobPointer, currentTime - timeWorked, 
                          timeWorked);
        int lrQueue = (workingQueue + 1 == numQueues) ? workingQueue : workingQueue + 1;
        queues[lrQueue].splice(queues[lrQueue].end(), queues[workingQueue], working);
        isWorking = false;
        timeWorked = 0; // reset time worked
      }
    }
    
    // 3. if next ready job has arrived given the current time, add to topmost queue
    this->addToJobQueue(schedulerJobs, totalJobs, &remainingJobs, currentTime, &queues[0]);
    
    // 4. if all jobs are finished
    if (!isWorking && remainingJobs == 0) {
      running = false;
      for (int i = 0; i < numQueues && !running; i++) {
        running = !queues[i].empty();
      }
    }
    
    currentTime++; // inc time
    /*if (currentTime > 200) {
      break;
    }
    printf("ct:%d tj:%d rj:%d \n", currentTime, totalJobs, remainingJobs);
    printf("q1s:%d q2s:%d q3s:%d \n", queues[0].size(), queues[1].size(), queues[2].size());
    */
    
    
  }
}

//====================== Print function ======================//
void scheduler::print() {
  int tatSum = 0;
  int wtSum  = 0;

	printf("---------- + ---- + -------- + -------- + -------- + ---------- + ----------- + ----------\n");
  printf("%-10s | %-4s | %-8s | %-8s | %-8s | %-10s | %-11s | %-10s\n", "Process #", "ID", "Priority", "Arrival", "Burst", "Completion", "Turn-Around", "Waiting"); 
  printf("---------- + ---- + -------- + -------- + -------- + ---------- + ----------- + ----------\n");

  for (int i = 0; i < this->size(); i++) {
  	result* element = this->getAt(i);
  	job* completedJob = element->completedJob;
  	 printf("%-10d | %-4d | %-8d | %-8d | %-8d | %-10d | %-11d | %-10d\n",
  	 	completedJob->processNum, completedJob->jobID, completedJob->priority, completedJob->arrivalTime, completedJob->burstTime,
  	 	element->completionTime, element->turnAroundTime, element->waitingTime);

    tatSum += element->turnAroundTime;
    wtSum  += element->waitingTime;
  }

  printf("---------- + ---- + -------- + -------- + -------- + ---------- + ----------- + ----------\n\n");

  printf("AVERAGE TURN-AROUND TIME: %d\nAVERAGE WAITING TIME: %d\n", tatSum/this->size(), wtSum/this->size());
}

std::string scheduler::printToFile() {
  std::ofstream outputFile;
  std::string filename = "output/results.csv";

  // create and open the csv
  outputFile.open(filename, std::ios::out);

  // throw error if file not open
  if(!outputFile) { std::cerr << "Can't open output file."<<std::endl; exit(1); }
  
  // write headers
  outputFile << "Process"     << "," 
             << "ID"          << "," 
             << "Priority"    << "," 
             << "Arrival"     << ","
             << "Burst"       << "," 
             << "Completion"  << "," 
             << "Turn-Around" << "," 
             << "Waiting"     << std::endl;

  // write data to file
  for (int i = 0; i < this->size(); i++) {
    result* element = this->getAt(i);
    job* completedJob = element->completedJob;
    outputFile << completedJob->processNum  << ","
               << completedJob->jobID       << "," 
               << completedJob->priority    << "," 
               << completedJob->arrivalTime << "," 
               << completedJob->burstTime   << ","
               << element->completionTime   << "," 
               << element->turnAroundTime   << "," 
               << element->waitingTime      << std::endl;
  }

  // close file
  outputFile.close();

  return filename;
}

void scheduler::guiPrint(std::string pathToCSV) {}

void scheduler::gantt() {
  int gTime = 0;

  printf("\nGANTT Chart\n");

  for (int i=0; i < this->history.size(); i++) {
    historyObj  element   = this->history[i];
    job*        job       = element.orgJbPntr;
    int         startTime = element.stTime;
    int         burst     = element.burst;

    if (gTime < startTime) {
      printf("+-----------+\n| ti = %-4d |\n|           |\n|           |\n| tf = %-4d |\n", gTime, startTime);
      gTime = startTime;

    } else if (gTime > startTime) {
      printf("\nERROR: Job begins while another job is executing, I think?\n");
      return;
    }

    printf("+-----------+\n| ti = %-4d |\n| P# = %-4d |\n| ID = %-4d |\n| tf = %-4d |\n", gTime, job->processNum, job->jobID, gTime+burst);

    gTime += burst;
  }

  printf("+-----------+\n");
}

std::string scheduler::ganttToFile() {
  std::ofstream outputFile;
  std::string filename = "output/gantt.csv";

  // create and open the csv
  outputFile.open(filename, std::ios::out);

  // throw error if file not open
  if(!outputFile) { std::cerr << "Can't open output file."<<std::endl; exit(1); }
  
  // write headers
  outputFile << "Process"     << "," 
             << "ID"          << "," 
             << "Start"       << "," 
             << "End"         << std::endl;

  // write data to file
  int gTime = 0;
  for (int i=0; i < this->history.size(); i++) {
    historyObj  element   = this->history[i];
    job*        job       = element.orgJbPntr;
    int         startTime = element.stTime;
    int         burst     = element.burst;

    if (gTime < startTime) {
      outputFile << ""        << ","
                 << ""        << "," 
                 << gTime     << "," 
                 << startTime << std::endl;
      gTime = startTime;

    } else if (gTime > startTime) {
      printf("\nERROR: Job begins while another job is executing, I think?\n");
      exit(1);
    }

    outputFile << job->processNum << ","
               << job->jobID      << "," 
               << gTime           << "," 
               << gTime+burst     << std::endl;
    gTime += burst;
  }

  // close file
  outputFile.close();

  return filename;
}

void scheduler::guiGantt(std::string pathToCSV) {

}

//====================== Clear functions ======================//
void scheduler::clearScheduler() {
  this->resultsList.clear();
  this->history.clear();
}
