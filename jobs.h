#ifndef JOBS_H
#define JOBS_H

#include <vector>
#include <cstring>

// job object for a sample job
struct job {
  int processNum;  // 0
  int jobID;       // 1
  int arrivalTime; // 2
  int burstTime;   // 3
  int priority;    // 4
};

// jobs wrapper object. Manages the import and storage of jobs as they come
//   in for the scheduler
class jobs {

private:
  // vector job list
  // If we keep it as job*, the get functions can return NULL if it fails. We
  //   can use unique pointers here but since you're not familiar with them it
  //   would probably be better using standard pointers and not messing with
  //   deletion.
  std::vector<job*> jobList;
  
  // compares
  static bool compByProcess(job*, job*);
  static bool compByArrival(job*, job*);
  static bool compByBurst(job*, job*);

public:
  jobs();
  ~jobs();
  
  // parses input file.
  bool init(char*); // Returns success/fail.
  
  // adds job to end of list
  void push(int, int, int);
  void push(job*);
  
  // gets a job pointer for the requested index
  job* getAt(int);
  job* getTop();

  // print job table
  void print();
  
  // returns size of list
  int size();
  
  // sorts the list based on 1:process, 2:arrivalT, 3:burstT
  bool sort(int); // Returns success/fail
  static bool compare(job*, job*, int);
  
  // clears the list
  void clearList();
};

#endif
