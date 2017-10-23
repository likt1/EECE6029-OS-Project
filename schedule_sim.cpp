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
  }
  else {
    printf("Error opening file specified.\n");
  }
  
  return 0;
}
