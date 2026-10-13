#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_PROCESSES 20
#define MAX_GANTT 500

typedef struct {
    int pid;
    int arrival;
    int burst;
    int priority;      // lower number = higher priority
    int remaining;
    int start_time;    // -1 until first CPU use
    int completion;
    int waiting;
    int turnaround;
    int response;
} Process;

typedef struct {
    int pid;           // -1 for CPU idle
    int start;
    int end;
} GanttEntry;

int fcfs(Process p[], int n, GanttEntry g[]);
int sjf_np(Process p[], int n, GanttEntry g[]);
int srtf(Process p[], int n, GanttEntry g[]);
int priority_np(Process p[], int n, GanttEntry g[]);
int priority_p(Process p[], int n, GanttEntry g[]);
int round_robin(Process p[], int n, int quantum, GanttEntry g[]);

void print_gantt(GanttEntry g[], int count);
void calculate_metrics(Process p[], int n);
void print_metrics(Process p[], int n);

#endif