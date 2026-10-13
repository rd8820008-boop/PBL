
#include <stdio.h>
#include "scheduler.h"

int round_robin(Process p[], int n, int quantum, GanttEntry g[])
{
    if (n <= 0 || quantum <= 0)
        return 0;

    int queue[MAX_PROCESSES * 500];
    int front = 0, rear = 0;
    int time = 0;
    int completed = 0;
    int gantt_count = 0;
    int added[MAX_PROCESSES] = {0};

    // Initialize process information
    for (int i = 0; i < n; i++) {
        p[i].remaining = p[i].burst;
        p[i].start_time = -1;
        p[i].completion = 0;
        p[i].waiting = 0;
        p[i].turnaround = 0;
        p[i].response = 0;
    }

    // Start from the earliest arrival time
    time = p[0].arrival;
    for (int i = 1; i < n; i++) {
        if (p[i].arrival < time)
            time = p[i].arrival;
    }

    // Add processes that have arrived
    for (int i = 0; i < n; i++) {
        if (p[i].arrival <= time) {
            queue[rear++] = i;
            added[i] = 1;
        }
    }

    while (completed < n && gantt_count < MAX_GANTT) {

        // If the ready queue is empty, CPU is idle
        if (front == rear) {
            int next_arrival = -1;

            for (int i = 0; i < n; i++) {
                if (!added[i] &&
                    (next_arrival == -1 ||
                     p[i].arrival < next_arrival)) {
                    next_arrival = p[i].arrival;
                }
            }

            if (next_arrival == -1)
                break;

            g[gantt_count].pid = -1;
            g[gantt_count].start = time;
            g[gantt_count].end = next_arrival;
            gantt_count++;

            time = next_arrival;

            for (int i = 0; i < n; i++) {
                if (!added[i] && p[i].arrival <= time) {
                    queue[rear++] = i;
                    added[i] = 1;
                }
            }

            continue;
        }

        int index = queue[front++];

        if (p[index].start_time == -1)
            p[index].start_time = time;

        int run_time = p[index].remaining < quantum
                       ? p[index].remaining : quantum;

        int start = time;
        time += run_time;
        p[index].remaining -= run_time;

        g[gantt_count].pid = p[index].pid;
        g[gantt_count].start = start;
        g[gantt_count].end = time;
        gantt_count++;

        // Add newly arrived processes before re-queuing
        // the process whose time slice just ended.
        for (int i = 0; i < n; i++) {
            if (!added[i] && p[i].arrival <= time) {
                queue[rear++] = i;
                added[i] = 1;
            }
        }

        if (p[index].remaining == 0) {
            p[index].completion = time;
            completed++;
        } else {
            queue[rear++] = index;
        }
    }

    return gantt_count;
}
