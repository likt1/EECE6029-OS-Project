#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "jobs.h"
#include <vector>
#include <list>
#include <string>

#define queueObjList std::list<queueObj>

// Helper structs
struct queueObj {
  job* originalJobPointer;
  job queueJob;
  int insertTime;
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
	std::vector<result> resultsList;
	std::vector<historyObj> history;

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
	std::string printToFile(std::string filename);
	void guiPrint(std::string pathToCSV);

	void gantt();
	std::string ganttToFile(std::string filename);
	void guiGantt(std::string pathToCSV);

	// clears the results and history
  void clearScheduler();

  // returns implemented schedulers
  // IMPORTANT, MUST UPDATE WHENEVER WE ADD A NEW SCHEDULER
  int numSchedulers();

	// schedulers
	// First-In First Out
	void FIFO(jobs*);

	// Lottery
	void lottery(jobs*);

	// Shortest Job First
	void idealSJF(jobs*);
	void realSJF(jobs*, double, int);
	
	// Round Robin
	void roundRobin(jobs*, int);
	
	// Age Based Priority
	void ageBasedPri(jobs*, int);
	int calcPri(int, job*, int);
	
	// Multi Lvl Feedback Queue
	void MLFQ(jobs*, int, int*, int);
	
	// Shared code between schedulers
	void addToJobQueue(jobs*, int, int*, int, queueObjList*);
};

#endif
