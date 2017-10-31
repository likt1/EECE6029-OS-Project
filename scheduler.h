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

struct historyObj {
  job* orgJbPntr;
  int stTime;
  int burst;
};

class scheduler {

private:
	std::vector<result*> resultsList;
	std::vector<historyObj*> history;

public:
	scheduler();
	~scheduler();

	// adds result to end of list, calculates TAT and WT
	void push(job*, int);

	// appends job history object to history list
	void pushHistory(job*, int, int);

	// returns size of result list
	int size();

	// gets a job pointer for the requested index
	result* getAt(int);

	// outputs
	void print();
	void gantt();

	// clears the results and history
    void clearScheduler();

	// schedulers
	// First-In First Out
	void FIFO(jobs*);

	// Shortest Job First
	void idealSJF(jobs*);
	void realSJF(jobs*, int);
	
	// Round Robin
	void roundRobin(jobs*);
	
	// Age Based Priority
	void ageBasedPri(jobs*);
	int calcPri(int, job*, int);
	
	// Shared code between schedulers
	void addToJobQueue(jobs*, int, int*, int, queueObjList*);
};

#endif
