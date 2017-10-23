# Benchmarking Schedulers
EECE6029 OS Project

Darius Cepulis & Kendrick Li

## Objective
Write scheduler(s) in C/C++ to demonstrate implementation of schedulers and simulate various workloads to benchmark.

## Goals
* To characterize the properties of different schedulers mentioned in class
* To show a simulation of them in action
* To show the pros/cons of each
* To compare them with modern schedulers

## Program Flow
1. Define load to run
2. Define schedulers to run
3. **RUN**
4. Receive graphical output

## Schedulers
1. First-In First-Out
2. Shortest Job First
	* Prediction-Based
3. Round Robin
4. Age-Based Priority
5. Multilevel Feedback Queue
  * Aging by moving around queues
  * 3 Queues Minimum
6. Homemade Linux CFS

## Things to Write
* Five or Six Schedulers
* Input Interface
* Output Graphs

## Input Interface
* File Reads
* CSV: Process Number, Arrival Time, Burst Time, Priority (Optional)
* Argument: Time Quantum

## Additional Design Goals
* Creativity is Important
* Test on Multiple Systems
	* Including Single Core v. Multi Core
* Define Test Suite
* Alpha Test Internally, Beta Test with Professor & Peers
* Provide Gantt Charts 
* Provide Complexity Analysis