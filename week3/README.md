# Process Scheduling Simulator (Week 3)

A C program that simulates six CPU scheduling algorithms and prints a Gantt chart and performance metrics for each.

## Algorithms
1. FCFS (First Come First Serve)
2. SJF (non-preemptive)
3. SRTF (Shortest Remaining Time First)
4. Priority (non-preemptive)
5. Priority (preemptive)
6. Round Robin (user-defined time quantum)

## Files
- scheduler.h: structs, limits, function prototypes (Team Leader)
- main.c: menu, input handling, calls the algorithms (Team Leader)
- metrics.c: waiting, turnaround and response times, averages (Team Leader)
- fcfs.c: FCFS and SJF (Inchara)
- strf.c: SRTF and Priority, both versions (Trisha)
- round_robin.c and gantt.c: Round Robin and Gantt chart printing (Anush)
- Makefile: build script (Rachel)

## How to build and run
Open a terminal in the week3 folder and run:

    make clean && make
    ./scheduler

## Menu
- 1 to 6: run a single algorithm
- 7: run all six (asks for a time quantum)
- 8: enter your own processes (arrival, burst, priority)
- 9: load the sample data
- 0: exit

Lower priority number means higher priority.

## Metrics
- Turnaround time = completion - arrival
- Waiting time = turnaround - burst
- Response time = first CPU start - arrival

## Test cases
Outputs are saved in output_test1.txt, output_test2.txt and output_test3.txt.