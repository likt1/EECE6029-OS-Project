#include "jobs.h"

#include <fstream>
#include <string>
#include <algorithm>

#include "stdio.h"

//====================== Init functions ======================//
jobs::jobs() {
}

jobs::~jobs() {
  clearList();
}

// I've decided to keep this in as using fopen and fread would contain basically
//   the same amount of code.
bool jobs::init(char* filename) {
  std::string line;
  std::ifstream file(filename);
  
  bool mangled = false;
  if (file.is_open()) {
    int jobID = 0;
    while (getline(file, line)) {
      // mangled input checker
      mangled = line.find('/') != std::string::npos; // ignore comments in csv
      
      if (!mangled) { 
        char* tmp = new char[line.length() + 1]; // create new cstring
        std::strcpy(tmp, line.c_str()); // move string into cstring

        job* newJob = new job();
        newJob->jobID = jobID;

        int cnt = 0;

        char* tok = std::strtok(tmp, ","); // tokenize off commas
        while (tok != 0) {
          int val = atoi(tok); // atoi ignores non-int values
          switch(cnt) { // if there are lots of commas, only take the first 3
            case 0:
              newJob->processNum = val;
              break;
            case 1:
              newJob->arrivalTime = val;
              break;
            case 2:
              newJob->burstTime = val;
              break;
            case 3:
              newJob->priority = val;
              break;
          }
          
          cnt++;
          tok = std::strtok(NULL,",");
        }
        
        this->push(newJob);
        jobID++;
        delete[] tmp;
      }
    }
    file.close();
    
    return true;
  }
  return false;
}

//====================== Push functions ======================//
// Add job to end of list
// The list manages all pointers assigned to it. Do not delete pointers assigned
//   to any part of the list.
void jobs::push(job* newJob) {
  this->jobList.push_back(newJob);
}

void jobs::push(int pn, int at, int bt) {
  job* newJob = new job();
  newJob->processNum = pn;
  newJob->arrivalTime = at;
  newJob->burstTime = bt;
  
  this->jobList.push_back(newJob);
}

//====================== Access functions ======================//
job* jobs::getAt(int index) {
  if (index < this->jobList.size()) {
    return this->jobList[index];
  }
  
  return NULL;
}

job* jobs::getTop() {
  if (!this->jobList.empty()) {
    return getAt(0);
  }
  
  return NULL;
}

int jobs::size() {
  return this->jobList.size();
}


//====================== Print function ======================//
void jobs::print() {
  printf("---------- + ---- + -------- + -------- + -------- \n");
  printf("%-10s | %-4s | %-8s | %-8s | %-8s \n", "Process #", "ID", "Priority", "Arrival", "Burst"); 
  printf("---------- + ---- + -------- + -------- + -------- \n");

  for (int i = 0; i < this->size(); i++) {
    job* element = this->getAt(i);

    printf("%-10d | %-4d | %-8d | %-8d | %-8d\n",
        element->processNum, element->jobID, element->priority, element->arrivalTime, element->burstTime);
  }


  printf("---------- + ---- + -------- + -------- + -------- \n");
}

//====================== Sort functions ======================//
bool jobs::sort(int method) {
  switch(method) {
    case 1:
      std::sort(this->jobList.begin(), this->jobList.end(), compByProcess);
      return true;
      break;
    case 2:
      std::sort(this->jobList.begin(), this->jobList.end(), compByArrival);
      return true;
      break;
    case 3:
      std::sort(this->jobList.begin(), this->jobList.end(), compByBurst);
      return true;
      break;
  }
  
  // method out of range
  return false;
}

//====================== Compare functions ======================//
// Currently works off a set order pn->at->bt, at->pn->bt, bt->at->pn. Can 
//   improve to a custom set form but this works for now and we
//   don't need that functionality yet.
bool jobs::compByProcess(job* first, job* sec) {
  return compare(first, sec, 1);
}

bool jobs::compByArrival(job* first, job* sec) {
  return compare(first, sec, 2);
}

bool jobs::compByBurst(job* first, job* sec) {
  return compare(first, sec, 3);
}

bool jobs::compare(job* first, job* sec, int mode) {
  std::vector<int> order = {2, 0, 3, 1, 4}; // by arrival time is default
  switch(mode) { // set up the order of which to check
    case 1: // by process num
      order = {0, 2, 3, 1, 4};
      break;
    case 3: // by burst time
      order = {3, 2, 0, 1, 4};
      break;
  }
  
  bool result = false;
  for (int i = 0; i < order.size() && !result; i++) {
    switch(order[i]) {
      case 0: // comp process
        result = first->processNum < sec->processNum;
        if (!result && first->processNum != sec->processNum) {
          return false;
        }
      break;
      case 1: // comp ID
        result = first->jobID < sec->jobID;
        if (!result && first->jobID != sec->jobID) {
          return false;
        }
      break;
      case 2: // comp arrival
        result = first->arrivalTime < sec->arrivalTime;
        if (!result && first->arrivalTime != sec->arrivalTime) {
          return false;
        }
      break;
      case 3: // comp burst
        result = first->burstTime < sec->burstTime;
        if (!result && first->burstTime != sec->burstTime) {
          return false;
        }
      break;
      case 4: // comp priority
        result = first->priority < sec->priority;
        if (!result && first->priority != sec->priority) {
          return false;
        }
      break;
    }
  }
  
  return result;
}

//====================== Clear functions ======================//
void jobs::clearList() {
  for (int i = 0; i < this->jobList.size(); i++) {
    delete this->jobList[i];
  }
  this->jobList.clear();
}
