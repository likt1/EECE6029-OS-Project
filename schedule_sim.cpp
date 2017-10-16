#include "stdio.h"

#include "jobs.h"
 
int main(int argc, char* argv[]) {
  jobs test;
  int success = 0;
  
  if (argc > 0) {
    success = test.init(argv[1]);
  }
  
  if (success) {
    for (int i = 0; i < test.size(); i++) {
      job* element = test.getAt(i);
      printf("Process Num: %d, Arrival Time: %d, Burst Time: %d\n",
        element->processNum, element->arrivalTime, element->burstTime);
    }
  }
  else {
    printf("Error opening file specified.\n");
  }
  
  return 0;
}
