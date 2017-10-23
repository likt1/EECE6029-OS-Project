#include "jobs.h"

#include <fstream>
#include <string>
#include <algorithm>

#include <cstring>
#include <cstdlib>

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
  if (file.is_open()) {
    while (getline(file, line)) {
      if (line.find('/') == std::string::npos) { // ignore comments in csv
        char* tmp = new char[line.length() + 1]; // create new cstring
        std::strcpy(tmp, line.c_str()); // move string into cstring

        job* newJob = new job();

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
          }
          
          cnt++;
          tok = std::strtok(NULL,",");
        }

        this->push(newJob);
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
  if (first->processNum == sec->processNum) {
    if (first->arrivalTime == sec->arrivalTime) {
      return compByBurst(first, sec);
    }
    return compByArrival(first, sec);
  }
  return first->processNum < sec->processNum;
}

bool jobs::compByArrival(job* first, job* sec) {
  if (first->arrivalTime == sec->arrivalTime) {
    if (first->processNum == sec->processNum) {
      return compByBurst(first, sec);
    }
    return compByProcess(first, sec);
  }
  return first->arrivalTime < sec->arrivalTime;
}

bool jobs::compByBurst(job* first, job* sec) {
  if (first->burstTime == sec->burstTime) {
    if (first->arrivalTime == sec->arrivalTime) {
      return compByProcess(first, sec);
    }
    return compByArrival(first, sec);
  }
  return first->burstTime < sec->burstTime;
}

bool jobs::equals(job* first, job* sec) {
  return first->processNum == sec->processNum &&
    first->arrivalTime == sec->arrivalTime &&
    first->burstTime == sec->burstTime;
}

bool jobs::compare(job* first, job* sec, int mode) {
  if (equals(first, sec)) {
    return false;
  }
  
  switch(mode) {
    case 1:
      return compByProcess(first, sec);
    case 2:
      return compByArrival(first, sec);
    case 3:
      return compByBurst(first, sec);
  }
  
  // mode out of range
  return false;
}

//====================== Clear functions ======================//
void jobs::clearList() {
  for (int i = 0; i < this->jobList.size(); i++) {
    delete this->jobList[i];
  }
  this->jobList.clear();
}
