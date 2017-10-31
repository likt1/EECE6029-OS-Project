#include "scheduler.h"
#include "jobs.h"

#include "stdio.h"

scheduler::scheduler()  {}

scheduler::~scheduler() { 
	clearScheduler(); 
}

//====================== Push functions ======================//
// Add job to end of list
// The list manages all pointers assigned to it. Do not delete pointers assigned
//   to any part of the list.

void scheduler::push(job* job, int ct) {
  result* newResult = new result();
  newResult->completedJob   = job;
  newResult->completionTime = ct;
  newResult->turnAroundTime = ct - (job->arrivalTime);
  newResult->waitingTime    = (newResult->turnAroundTime) - (job->burstTime);
  
  this->resultsList.push_back(newResult);
}

void scheduler::pushHistory(job* job, int startTime, int burst) {
  historyObj* newHistory = new historyObj();
  newHistory->orgJbPntr  = job;
  newHistory->stTime     = startTime;
  newHistory->burst      = burst;

  this->history.push_back(newHistory);
}

//====================== Access functions ======================//
result* scheduler::getAt(int index) {
  if (index < this->resultsList.size()) {
    return this->resultsList[index];
  }
  
  return NULL;
}

int scheduler::size() {
  return this->resultsList.size();
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

// addToJobQueue is shared between RR and TB-PQ
void scheduler::addToJobQueue(jobs* sj, int tj, int* rj, int ct, queueObjList* q) {
  if (*rj > 0) {
    job* nextReadyJob = sj->getAt(tj - *rj); // get next ready job
    
    // if that job arrived at the current time
    if (nextReadyJob->arrivalTime == ct) { 
      queueObj temp; // construct queueJob
      temp.originalJobPointer = nextReadyJob;
      temp.queueJob = *nextReadyJob;
    
      q->push_back(temp); // create a copy and insert into queue
      (*rj)--;
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
  schedulerJobs->sort(2); // sort on arrival time

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
  schedulerJobs->sort(2); // sort on arrival time

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
      for (next; next != queue.end(); ++next) {
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

//====================== Print function ======================//
void scheduler::print() {
	printf("---------- + ---- + -------- + -------- + -------- + ---------- + ----------- + ----------\n");
  printf("%-10s | %-4s | %-8s | %-8s | %-8s | %-10s | %-11s | %-10s\n", "Process #", "ID", "Priority", "Arrival", "Burst", "Completion", "Turn-Around", "Waiting"); 
  printf("---------- + ---- + -------- + -------- + -------- + ---------- + ----------- + ----------\n");

  for (int i = 0; i < this->size(); i++) {
  	result* element = this->getAt(i);
  	job* completedJob = element->completedJob;
  	 printf("%-10d | %-4d | %-8d | %-8d | %-8d | %-10d | %-11d | %-10d\n",
  	 	completedJob->processNum, completedJob->jobID, completedJob->priority, completedJob->arrivalTime, completedJob->burstTime,
  	 	element->completionTime, element->turnAroundTime, element->waitingTime);
  }

  printf("---------- + ---- + -------- + -------- + -------- + ---------- + ----------- + ----------\n");
}

void scheduler::gantt() {
  int gTime = 0;

  printf("\nGANTT beta\n");

  for (int i=0; i < this->history.size(); i++) {
    historyObj* element   = this->history[i];
    job*        job       = element->orgJbPntr;
    int         startTime = element->stTime;
    int         burst     = element->burst;

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

//====================== Clear functions ======================//
void scheduler::clearScheduler() {
  for (int i = 0; i < this->resultsList.size(); i++) {
    delete this->resultsList[i];
  }
  this->resultsList.clear();

  for (int i = 0; i < this->history.size(); i++) {
    delete this->history[i];
  }
  this->history.clear();
}
