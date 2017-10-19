#include "stdio.h"

#include "jobs.h"
 
int main(int argc, char* argv[]) {
  jobs schedulerJobs;
  bool success = false;
  
  if (argc > 0) {
    success = schedulerJobs.init(argv[1]);
  }
  
  if (success) {
    for (int i = 0; i < schedulerJobs.size(); i++) {
      job* element = schedulerJobs.getAt(i);
      printf("Process Num: %d, Arrival Time: %d, Burst Time: %d\n",
        element->processNum, element->arrivalTime, element->burstTime);
    }
    printf("\nAfter sorting by Process Number Time:\n\n");
    schedulerJobs.sort(1);
    for (int i = 0; i < schedulerJobs.size(); i++) {
      job* element = schedulerJobs.getAt(i);
      printf("Process Num: %d, Arrival Time: %d, Burst Time: %d\n",
        element->processNum, element->arrivalTime, element->burstTime);
    }
  }
  else {
    printf("Error opening file specified.\n");
  }
  
  return 0;
}
