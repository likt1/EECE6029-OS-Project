#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "jobs.h"
#include <vector>
#include <list>

#define queueObjList std::list<queueObj>

// Helper structs
struct queueObj {
  job* originalJobPointer;
  job queueJob;
};

struct result {
	job* completedJob;
	int  completionTime;
	int  turnAroundTime;
	int  waitingTime;
};

class scheduler {

private:
	std::vector<result*> resultsList;

public:
	scheduler();
	~scheduler();

	// adds result to end of list, calculates TAT and WT
	void push(job*, int);

	// returns size of result list
	int size();

	// gets a job pointer for the requested index
	result* getAt(int);

	// outputs
	void print();
	void gantt();

	// clears the list
  void clearList();

	// schedulers
	// FIFO
	void FIFO(jobs*);
	
	// Round Robin
	void roundRobin(jobs*);
	
	// Age Based Priority
	void ageBasedPri(jobs*);
	int calcPri(int, job*, int);
	
	// Shared code between schedulers
	void addToJobQueue(jobs*, int, int*, int, queueObjList*);
};

#endif
