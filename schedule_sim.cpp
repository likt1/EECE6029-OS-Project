#include "stdio.h"

#include "jobs.h"
 
void FIFO(jobs*);

int main(int argc, char* argv[]) {
  jobs schedulerJobs;
  bool success = false;
  
  if (argc > 0) {
    success = schedulerJobs.init(argv[1]);
  }
  
  if (success) {
    schedulerJobs.print();

    printf("\nAfter FIFO:\n");
    FIFO(&schedulerJobs);
  }
  else {
    printf("Error opening file specified.\n");
  }
  
  return 0;
}

// To do: move this out to its own function 'n stuff. How? TBD 10/23
void FIFO(jobs* schedulerJobs) {
      //FIFO is going to be very simple. Just keep dequeueing the sorted queue.
    schedulerJobs->sort(2);

    int completionTime  = schedulerJobs->getTop() -> arrivalTime;
    int completionTimes [schedulerJobs->size()];
    int turnAroundTimes [schedulerJobs->size()];
    int waitingTimes    [schedulerJobs->size()];

    printf("---------- + ---- + -------- + -------- + -------- + ---------- + ----------- + ----------\n");
    printf("%-10s | %-4s | %-8s | %-8s | %-8s | %-10s | %-11s | %-10s\n", "Process #", "ID", "Priority", "Arrival", "Burst", "Completion", "Turn-Around", "Waiting"); 
    printf("---------- + ---- + -------- + -------- + -------- + ---------- + ----------- + ----------\n");

    for (int i = 0; i < schedulerJobs->size(); i++) {
      job* nextJob = schedulerJobs->getAt(i);
      printf("%-10d | %-4d | %-8d | %-8d | %-8d | ",
      nextJob->processNum, nextJob->jobID, nextJob->priority, nextJob->arrivalTime, nextJob->burstTime);

      completionTime  += nextJob->burstTime;

      completionTimes[i] = completionTime;
      turnAroundTimes[i] = completionTime - (nextJob->arrivalTime);
      waitingTimes[i]    = turnAroundTimes[i] - (nextJob->burstTime);

      printf("%-10d | %-11d | %-10d\n",
        completionTimes[i], turnAroundTimes[i], waitingTimes[i]);
    }

    printf("---------- + ---- + -------- + -------- + -------- + ---------- + ----------- + ----------\n");
}
