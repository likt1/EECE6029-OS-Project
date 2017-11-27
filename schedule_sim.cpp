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
void runSchedule(scheduler* scheduler, jobs* jobs, int type, int arg, double dArg, char* sArg) {
  switch(type) { // switch between modes
    case 0:
      printf("\nAfter FIFO:\n");
      scheduler->FIFO(jobs);
      scheduler->print();
      scheduler->guiGantt(scheduler->ganttToFile("gantt_fifo.csv"));
      scheduler->guiPrint(scheduler->printToFile("results_fifo.csv"));
      break;
    case 1:
      printf("\nAfter Round Robin\nTime quantum = %d:\n", arg);
      if (arg > 0) {
        scheduler->roundRobin(jobs, arg);
        scheduler->print();
        scheduler->guiGantt(scheduler->ganttToFile("gantt_rr.csv"));
        scheduler->guiPrint(scheduler->printToFile("results_rr.csv"));
      }
      else {
        printf("\nTime quantum must be larger than 0\n");
      }
      break;
    case 2:
      printf("\nAfter Age Based Pri\nMax wait for priority upgrade = %d:\n", arg);
      if (arg > 0) {
        scheduler->ageBasedPri(jobs, arg);
        scheduler->print();
        scheduler->guiGantt(scheduler->ganttToFile("gantt_abp.csv"));
        scheduler->guiPrint(scheduler->printToFile("results_abp.csv"));
      }
      else {
        printf("\nMax wait for priority upgrade must be larger than 0\n");
      }
      break;
    case 3:
      printf("\nAfter Ideal SJF:\n");
      scheduler->idealSJF(jobs);
      scheduler->print();
      scheduler->guiGantt(scheduler->ganttToFile("gantt_isjf.csv"));
      scheduler->guiPrint(scheduler->printToFile("results_isjf.csv"));
      break;
    case 4:
      // alpha is a smoothing factor between 0 and 1
      printf("\nAfter Predictive SJF\nLearning Speed = %f\n Initial BT Estimate = %d:\n", dArg, arg);
      if (dArg > 0 && dArg < 1) {
        scheduler->realSJF(jobs, dArg, arg);
        scheduler->print();
        scheduler->guiGantt(scheduler->ganttToFile("gantt_psjf.csv"));
        scheduler->guiPrint(scheduler->printToFile("results_psjf.csv"));
      }
      else {
        printf("\nLearning Speed must be between 0 and 1\n");
      }
      break;
    case 6:
      printf("\nAfter Lottery:\n");
      scheduler->lottery(jobs);
      scheduler->print();
      scheduler->guiGantt(scheduler->ganttToFile("gantt_lottery.csv"));
      scheduler->guiPrint(scheduler->printToFile("results_lottery.csv"));
      break;
    case 5:
      // get int array from string
      bool zeroFail = false;
      std::vector<int> temp;
      char* tok = std::strtok(sArg, ","); // tokenize off commas
      while (tok != 0) {
        int val = atoi(tok);
        if (val < 1) {
          zeroFail = true;
        }
        temp.push_back(val);
        tok = std::strtok(NULL,",");
      }
      
      int queueNum = temp.size();
      int queueTimeQuantum[queueNum];
      
      printf("\nAfter Multilvl Feedback Queue\nQueue number = %d\nPreemption = {", queueNum);
      for (int i = 0; i < queueNum; i++) {
        queueTimeQuantum[i] = temp[i];
        printf("%d,", queueTimeQuantum[i]);
      }
      printf("}:\nMax wait for queue upgrade = %d:\n", arg);
      
      if (queueNum > 1 && !zeroFail) {
        scheduler->MLFQ(jobs, queueNum, queueTimeQuantum, arg);
        scheduler->print();
        scheduler->guiGantt(scheduler->ganttToFile("gantt_mlfq.csv"));
        scheduler->guiPrint(scheduler->printToFile("results_mlfq.csv"));
      }
      else {
        printf("\nNumber of queues must be greater than 1 or preemption needs to be greater than 0\n");
      }
      break;
  }
}

// run a schedule on default input
void runScheduleDefault(scheduler* scheduler, jobs* jobs, int type) {
  switch(type) { // switch between modes
    case 0:
      printf("\nAfter FIFO:\n");
      scheduler->FIFO(jobs);
      scheduler->print();
      scheduler->guiGantt(scheduler->ganttToFile("gantt_fifo.csv"));
      scheduler->guiPrint(scheduler->printToFile("results_fifo.csv"));
      break;
    case 1:
      printf("\nAfter Round Robin\nTime quantum = 5:\n");
      scheduler->roundRobin(jobs, 5);
      scheduler->print();
      scheduler->guiGantt(scheduler->ganttToFile("gantt_rr.csv"));
      scheduler->guiPrint(scheduler->printToFile("results_rr.csv"));
      break;
    case 2:
      printf("\nAfter Age Based Pri\nMax wait for priority upgrade = 20:\n");
      scheduler->ageBasedPri(jobs, 20);
      scheduler->print();
      scheduler->guiGantt(scheduler->ganttToFile("gantt_abp.csv"));
      scheduler->guiPrint(scheduler->printToFile("results_abp.csv"));
      break;
    case 3:
      printf("\nAfter Ideal SJF:\n");
      scheduler->idealSJF(jobs);
      scheduler->print();
      scheduler->guiGantt(scheduler->ganttToFile("gantt_isjf.csv"));
      scheduler->guiPrint(scheduler->printToFile("results_isjf.csv"));
      break;
    case 4:
      // alpha is a smoothing factor between 0 and 1
      printf("\nAfter Predictive SJF\nLearning Speed = 0.5\n Initial BT Estimate = 10:\n");
      scheduler->realSJF(jobs, 0.5, 10);
      scheduler->print();
      scheduler->guiGantt(scheduler->ganttToFile("gantt_psjf.csv"));
      scheduler->guiPrint(scheduler->printToFile("results_psjf.csv"));
      break;
    case 6:
      printf("\nAfter Lottery:\n");
      scheduler->lottery(jobs);
      scheduler->print();
      scheduler->guiGantt(scheduler->ganttToFile("gantt_lottery.csv"));
      scheduler->guiPrint(scheduler->printToFile("results_lottery.csv"));
      break;
    case 5:
      printf("\nAfter Multilvl Feedback Queue\nQueue number = 3\nPreemption = {15, 30, 50}:\nMax wait for queue upgrade = 200:\n");
      int queueTimeQuantum[] = {15, 30, 50};
      scheduler->MLFQ(jobs, 3, queueTimeQuantum, 200);
      scheduler->print();
      scheduler->guiGantt(scheduler->ganttToFile("gantt_mlfq.csv"));
      scheduler->guiPrint(scheduler->printToFile("results_mlfq.csv"));
      break;
  }
}

int main(int argc, char* argv[]) {
  jobs      schedulerJobs;
  scheduler scheduledJobs;
  bool success = false; // if the file was successfully opened
  int mode = 0; // to run all schedulers or just some
  char* file = argv[1]; // first arg
  char* modeStr = argv[2]; // second arg
  
  if (argc > 1) { // we have at least one arg
    if (strMatch(file, "help")) { // print out help text if help
      bool fullText = false;
      if (argc == 3) { // command: schedule_sim.out help schedName
        if (strMatch(modeStr, "fifo")) {
          printf("Command input:\n");
          printf("fileName fifo\n\n");
        }
        else if(strMatch(modeStr, "rr")) {
          printf("Command input:\n");
          printf("fileName rr timeQuantum(not 0)\n\n");
        }
        else if(strMatch(modeStr, "abp")) {
          printf("Command input:\n");
          printf("fileName adp priorityUpWaitTime(not 0)\n\n");
        }
        else if(strMatch(modeStr, "isjf")) {
          printf("Command input:\n");
          printf("fileName isjf\n\n");
        }
        else if(strMatch(modeStr, "psjf")) {
          printf("Command input:\n");
          printf("fileName psjf learningSpeed(between 0 and 1) initialBTEstimate\n\n");
        }
        else if(strMatch(modeStr, "mlfq")) {
          printf("The number of preemption quantums entered gives the number of queues. (min 2)\n");
          printf("Command input:\n");
          printf("fileName mlfq preemptQuantum1,preemptQuantum2,... queueUpgradeWaitTime\n\n");
        }
        else if(strMatch(modeStr, "lottery")) {
          printf("Command input:\n");
          printf("fileName lottery\n\n");
        }
        else if(strMatch(modeStr, "sort")) {
          printf("Command input:\n");
          printf("fileName sort type(1, 2 or 3)\n\n");
        }
        else {
          printf("Unknown scheduler specified.\n");
          fullText = true;
        }
      }
      else {
        fullText = true;
      }
      
      if (fullText) { // if it doesn't fit with anything above, just show full help
        printf("Just specify a csv jobs file to run every scheduler.\n");
        printf("To get help on a specific scheduler, specify 'help' as the file name:\n");
        printf("To run a specific scheduler, add the following after the file name:\n");
        printf("fifo, rr, abp, isjf, psjf, mlfq, lottery\n\n");
        printf("Schedulers Implemented:\n");
        printf("First In First Out\nRound Robin\nAge Based Priority\nIdeal Shortest Job First\n\
Predictive Shortest Job First\nMultilevel Feedback Queue\nLottery.\n");
      }
      return 0; // terminate program
    }
    
    success = schedulerJobs.init(file);
    // 3 argc (schedule_sim.out filename jobtype)
    if (argc >= 3) {  // switch to job input mode
      mode = 1;
    }
  }
  
  if (success) { // only do stuff if file is successfully opened
    if (mode == 0) { // do everything
      schedulerJobs.print();

      for (int i = 0; i < scheduledJobs.numSchedulers(); i++) {
        runScheduleDefault(&scheduledJobs, &schedulerJobs, i);
      }
    }
    else if (mode == 1) { // do only one
      for (int i = 0; modeStr[i]; i++) { // convert arg to lowercase
        modeStr[i] = std::tolower(modeStr[i]);
      }
      
      schedulerJobs.print();
      if (strMatch(modeStr, "fifo")) {
        runScheduleDefault(&scheduledJobs, &schedulerJobs, 0);
      }
      else if(strMatch(modeStr, "rr")) {
        // check min 3 input parameters 
        if (argc > 3) {
          runSchedule(&scheduledJobs, &schedulerJobs, 1, atoi(argv[3]), 0, NULL);
        }
        else { // defualt for rr, timeQuantum = 5
          runScheduleDefault(&scheduledJobs, &schedulerJobs, 1);
        }
      }
      else if(strMatch(modeStr, "abp")) {
        // check min 3 input parameters 
        if (argc > 3) {
          runSchedule(&scheduledJobs, &schedulerJobs, 2, atoi(argv[3]), 0, NULL);
        }
        else { // defualt for adp, maxWait = 20
          runScheduleDefault(&scheduledJobs, &schedulerJobs, 2);
        }
      }
      else if(strMatch(modeStr, "isjf")) {
        runScheduleDefault(&scheduledJobs, &schedulerJobs, 3);
      }
      else if(strMatch(modeStr, "psjf")) {
        // check min 4 input parameters 
        if (argc > 4) {
          runSchedule(&scheduledJobs, &schedulerJobs, 4, atoi(argv[4]), atof(argv[3]), NULL);
        }
        else { // defualt for psjf, learningSpeed = 0.5 BTEst = 10
          runScheduleDefault(&scheduledJobs, &schedulerJobs, 4);
        }
      }
      else if(strMatch(modeStr, "mlfq")) {
        // check min 4 input parameters 
        if (argc > 4) {
          runSchedule(&scheduledJobs, &schedulerJobs, 5, atoi(argv[4]), 0, argv[3]);
        }
        else { // defualt for mlfq, numQueues = 3 preemptQuantums = 15,30,50 queueUpgradeWaitTime = 200
          runScheduleDefault(&scheduledJobs, &schedulerJobs, 5);
        }
      }
      else if(strMatch(modeStr, "lottery")) {
        runScheduleDefault(&scheduledJobs, &schedulerJobs, 6);
      }
      else if(strMatch(modeStr, "sort")) {
        if (argc > 3) {
          schedulerJobs.sort(atoi(argv[3]));
        }
        else {
          schedulerJobs.sort(1);
        }
        printf("Sorted.\n\n");
        schedulerJobs.print();
      }
      else {
        printf("Unknown scheduler specified.\nTry 'help' for more info.\n");
      }
    }
  }
  else {
    printf("Error opening file specified.\n");
  }
  
  return 0;
}
