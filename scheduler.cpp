#include "scheduler.h"
#include "jobs.h"

#include "stdio.h"

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

// Non-preemptive priority scheduler based on age
void scheduler::ageBasedPri(jobs* schedulerJobs) {
	this->clearList();
  schedulerJobs->sort(2);

  int currentTime = 0; // system clock
  int working = -1; // which job we are currently working on
  int totalJobs = schedulerJobs->size();
  int remainingJobs = totalJobs;
  std::vector<job> queue;

  while(remainingJobs > 0) {
    job* nextReadyJob = schedulerJobs->getAt(totalJobs - remainingJobs);
    // if next ready job has arrived given the current time, add to queue
    
    // if queue is not empty, work on highest priority
    
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
