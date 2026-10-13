#include <stdio.h>
#include "scheduler.h"

#define N 4
#define QUANTUM 2

// Test case 1: pid, arrival, burst, priority
static Process original[N] = {
    {1, 0, 5, 2}, {2, 1, 3, 1}, {3, 2, 8, 4}, {4, 3, 6, 3}
};

// Fresh copy so every algorithm starts clean
static void reset(Process work[]) {
    for (int i = 0; i < N; i++) {
        work[i] = original[i];
        work[i].remaining = work[i].burst;
        work[i].start_time = -1;
        work[i].completion = 0;
    }
}

static void finish(const char *name, Process work[], GanttEntry g[], int count) {
    printf("===== %s =====\n", name);
    print_gantt(g, count);
    calculate_metrics(work, N);
    print_metrics(work, N);
}

int main(void) {
    Process work[N];
    GanttEntry g[MAX_GANTT];
    int count;

    reset(work); count = fcfs(work, N, g);        finish("FCFS", work, g, count);
    reset(work); count = sjf_np(work, N, g);      finish("SJF (non-preemptive)", work, g, count);
    reset(work); count = srtf(work, N, g);        finish("SRTF", work, g, count);
    reset(work); count = priority_np(work, N, g); finish("Priority (non-preemptive)", work, g, count);
    reset(work); count = priority_p(work, N, g);  finish("Priority (preemptive)", work, g, count);
    reset(work); count = round_robin(work, N, QUANTUM, g); finish("Round Robin", work, g, count);

    return 0;
}