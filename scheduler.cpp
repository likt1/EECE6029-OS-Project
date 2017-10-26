#include "scheduler.h"
#include "jobs.h"

#include "stdio.h"
#include <list>

// Aged based priority helper structs
struct queueObj {
  job* originalJobPointer;
  job queueJob;
};

#define queueObjList std::list<queueObj>

scheduler::scheduler()  {}

scheduler::~scheduler() { 
	clearList(); 
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
	this->clearList();
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
	this->clearList();
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
    //  work on highest priority
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
    
    // work on job if we are working on a job (non-preemptive)
    if (isWorking) {
      (*working).queueJob.burstTime--;
      
      // if we complete a job then add to results
      if ((*working).queueJob.burstTime == 0) {
        this->push((*working).originalJobPointer, currentTime);
        queue.erase(working);
        isWorking = false;
      }
    }
    
    // if next ready job has arrived given the current time, add to queue
    if (remainingJobs > 0) {
      job* nextReadyJob = schedulerJobs->getAt(totalJobs - remainingJobs);
      if (nextReadyJob->arrivalTime == currentTime) {
        queueObj temp; // construct queueJob
        temp.originalJobPointer = nextReadyJob;
        temp.queueJob = *nextReadyJob;
      
        queue.push_back(temp); // create a copy and insert into queue
        remainingJobs--;
      }
    }
    
    // if all jobs are finished
    if (!isWorking && queue.empty()) {
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
	//TODO
}

//====================== Clear functions ======================//
void scheduler::clearList() {
  for (int i = 0; i < this->resultsList.size(); i++) {
    delete this->resultsList[i];
  }
  this->resultsList.clear();
}
