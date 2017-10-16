#include "jobs.h"

#include <fstream>
#include <string>
#include <cstring>
#include <stdlib.h>

jobs::jobs() {
}

jobs::~jobs() {
  this->clearList();
}

int jobs::init(char* filename) {
  std::string line;
  std::ifstream file(filename);
  if (file.is_open()) {
    while (getline(file, line)) {
      if (line.find('/') == std::string::npos) {
        char* tmp = new char[line.length() + 1];
        std::strcpy(tmp, line.c_str());

        job* newJob = new job();
        int cnt = 0;

        char* tok = std::strtok(tmp, ",");
        
        while (tok != 0) {
          int val = atoi(tok);
          
          switch(cnt) {
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
    
    return 1;
  }
  return 0;
}

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

job* jobs::getAt(int index) {
  if (index < this->jobList.size()) {
    return this->jobList[index];
  }
  
  return NULL;
}

int jobs::size() {
  return this->jobList.size();
}

void jobs::clearList() {
  for (int i = 0; i < this->jobList.size(); i++) {
    delete this->jobList[i];
  }
  this->jobList.clear();
}
