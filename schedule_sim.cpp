#include "stdio.h"

#include <cctype>

#include "jobs.h"
#include "scheduler.h"
 
// returns true if strings are identical
bool strMatch(const char* s1, const char* s2) {
  if (std::strlen(s1) != std::strlen(s2)) {
    return false;
  }
  
  return std::strncmp(s1, s2, std::strlen(s1)) == 0;
}

// runs a schedule on the specified scheduler with the input jobs
void runSchedule(scheduler* scheduler, jobs* jobs, int type) {
  switch(type) { // switch between modes
    case 0:
      printf("\nAfter FIFO:\n");
      scheduler->FIFO(jobs);
      scheduler->print();
      scheduler->ganttToFile();
      scheduler->printToFile();
      break;
    case 1:
      printf("\nAfter Round Robin:\n");
      scheduler->roundRobin(jobs);
      scheduler->print();
      scheduler->gantt();
      break;
    case 2:
      printf("\nAfter Age Based Pri:\n");
      scheduler->ageBasedPri(jobs);
      scheduler->print();
      scheduler->gantt();
      break;
    case 3:
      printf("\nAfter Ideal SJF:\n");
      scheduler->idealSJF(jobs);
      scheduler->print();
      scheduler->gantt();
      break;
    case 4:
      // alpha is a smoothing factor between 0 and 1
      printf("\nAfter Predictive SJF\nLearning Speed = 0.75\n Initial BT Estimate = 10:\n");
      scheduler->realSJF(jobs, 0.5, 10);
      scheduler->print();
      scheduler->gantt();
      break;
    case 5:
      // alpha is a smoothing factor between 0 and 1
      printf("\nAfter Multilvl Feedback Queue\nQueue number = 3\nPreemption = {15, 30, 50}:\n");
      scheduler->MLFQ(jobs);
      scheduler->print();
      scheduler->gantt();
      break;
  }
}

int main(int argc, char* argv[]) {
  jobs      schedulerJobs;
  scheduler scheduledJobs;
  bool success = false;
  int mode = 0;
  char* file = argv[1];
  char* modeStr = argv[2];
  
  if (argc > 1) { // we have at least one arg
    if (strMatch(file, "help")) { // print out help text
      printf("Just specify a csv jobs file to run every scheduler.\n");
      printf("To run a specific scheduler, add the following after the file name:\n");
      printf("fifo, rr, abp, isjf, psjf, mlfq\n\n");
      printf("Schedulers Implemented:\n");
      printf("First In First Out\nRound Robin\nAge Based Priority\nIdeal Shortest Job First\n\
Predictive Shortest Job First\nMultilevel Feedback Queue.\n");
      return 0;
    }
    
    success = schedulerJobs.init(file);
    // 3 argc (schedule_sim.out filename jobtype)
    if (argc == 3) {  // switch to job input mode
      mode = 1;
    }
  }
  
  if (success) {
    if (mode == 0) { // do everything
      schedulerJobs.print();

      for (int i = 0; i < scheduledJobs.numSchedulers(); i++) {
        runSchedule(&scheduledJobs, &schedulerJobs, i);
      }
    }
    else if (mode == 1) { // do only one
      for (int i = 0; modeStr[i]; i++) { // convert arg to lowercase
        argv[2][i] = std::tolower(modeStr[i]);
      }
      
      schedulerJobs.print();
      if (strMatch(modeStr, "fifo")) {
        runSchedule(&scheduledJobs, &schedulerJobs, 0);
      }
      else if(strMatch(modeStr, "rr")) {
        runSchedule(&scheduledJobs, &schedulerJobs, 1);
      }
      else if(strMatch(modeStr, "abp")) {
        runSchedule(&scheduledJobs, &schedulerJobs, 2);
      }
      else if(strMatch(modeStr, "isjf")) {
        runSchedule(&scheduledJobs, &schedulerJobs, 3);
      }
      else if(strMatch(modeStr, "psjf")) {
        runSchedule(&scheduledJobs, &schedulerJobs, 4);
      }
      else if(strMatch(modeStr, "mlfq")) {
        runSchedule(&scheduledJobs, &schedulerJobs, 5);
      }
      else {
        printf("Unknown scheduler specified.\nTry help for more info.\n");
      }
    }
  }
  else {
    printf("Error opening file specified.\n");
  }
  
  return 0;
}
