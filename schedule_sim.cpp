#include "stdio.h"

#include "jobs.h"
#include "scheduler.h"
 
void FIFO(jobs*);

int main(int argc, char* argv[]) {
  jobs      schedulerJobs;
  scheduler scheduledJobs;
  bool success = false;
  
  if (argc > 0) {
    success = schedulerJobs.init(argv[1]);
  }
  
  if (success) {
    schedulerJobs.print();

    printf("\nAfter FIFO:\n");
    scheduledJobs.FIFO(&schedulerJobs);
    scheduledJobs.print();
    scheduledJobs.gantt();
    
    printf("\nAfter Round Robin:\n");
    scheduledJobs.roundRobin(&schedulerJobs);
    scheduledJobs.print();
    scheduledJobs.gantt();
    
    printf("\nAfter Age Based Pri:\n");
    scheduledJobs.ageBasedPri(&schedulerJobs);
    scheduledJobs.print();
    scheduledJobs.gantt();

    printf("\nAfter Ideal SJF:\n");
    scheduledJobs.idealSJF(&schedulerJobs);
    scheduledJobs.print();
    scheduledJobs.gantt();

    // alpha is a smoothing factor between 0 and 1
    printf("\nAfter Predictive SJF\nLearning Speed = 0.75\nInitial BT Estimate = 10:\n");
    scheduledJobs.realSJF(&schedulerJobs, 0.5, 10);
    scheduledJobs.print();
    scheduledJobs.gantt();
  }
  else {
    printf("Error opening file specified.\n");
  }
  
  return 0;
}
