#ifndef JOBS_H
#define JOBS_H

#include <vector>

// job object for a sample job
struct job {
  int processNum;
  int arrivalTime;
  int burstTime;
};

// jobs wrapper object. Manages the import and storage of jobs as they come
//   in for the scheduler
class jobs {

private:
  // vector job list
  std::vector<job*> jobList;

public:
  jobs();
  ~jobs();
  
  // parses input file
  int init(char*);
  
  // adds job to end of list
  void push(int, int, int);
  void push(job*);
  
  // gets a job pointer for the requested index
  job* getAt(int);
  job* getTop();
  
  // returns size of list
  int size();
  
  // sorts the list based on 1:process, 2:arrivalT, 3:burstT
  void sort(int);
  
  // clears the list
  void clearList();
};

#endif
