#ifndef JOBS_H
#define JOBS_H

#include <vector>

struct job {
  int processNum;
  int arrivalTime;
  int burstTime;
};

class jobs {

private:
  std::vector<job*> jobList;

public:
  jobs();
  ~jobs();
  
  int init(char*);
  void push(int, int, int);
  void push(job*);
  job* getAt(int);
  int size();
  void clearList();
};

#endif
