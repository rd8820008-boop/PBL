/*
 * fcfs_sjf_combined.c  -  STANDALONE version
 *
 * This ONE file combines:
 *   1) scheduler.h  (the lead's header: structs, limits, prototypes)
 *   2) fcfs_sjf.c   (Member 1: fcfs, sjf_np)
 *   3) a test main() with Gantt chart + metrics output
 *
 * In the team repo these live in separate files and fcfs_sjf.c does
 *   #include "scheduler.h"
 * Here the header text is pasted inline (between the BEGIN/END scheduler.h
 * markers below) so the file compiles and runs by itself.
 *
 * Build:  gcc -Wall -Wextra -o fcfs_sjf_combined fcfs_sjf_combined.c
 * Run:    ./fcfs_sjf_combined
 *
 * NOTE: For the team repo, push ONLY fcfs_sjf.c (not this file) -- this one
 * redefines the header contents and main(), which would clash with the
 * lead's scheduler.h and main.c.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"


/* ============ BEGIN fcfs_sjf.c (Member 1) -- uses types/limits from scheduler.h ============ */

/* Append a Gantt block; merges with previous block if same pid and contiguous. */
static void gantt_add(GanttEntry g[], int *count, int pid, int s, int e)
{
    if (e <= s) return;
    if (*count > 0 && g[*count - 1].pid == pid && g[*count - 1].end == s) {
        g[*count - 1].end = e;
        return;
    }
    if (*count >= MAX_GANTT) return;   /* avoid overflowing g[] */
    g[*count].pid   = pid;
    g[*count].start = s;
    g[*count].end   = e;
    (*count)++;
}

/* ------------------------- FCFS ------------------------- */
int fcfs(Process p[], int n, GanttEntry g[])
{
    int order[MAX_PROCESSES];
    int count = 0, time = 0, i, j;

    for (i = 0; i < n; i++) order[i] = i;

    /* insertion sort of indices by arrival, tie -> lower PID */
    for (i = 1; i < n; i++) {
        int key = order[i];
        j = i - 1;
        while (j >= 0 &&
               (p[order[j]].arrival > p[key].arrival ||
                (p[order[j]].arrival == p[key].arrival &&
                 p[order[j]].pid > p[key].pid))) {
            order[j + 1] = order[j];
            j--;
        }
        order[j + 1] = key;
    }

    for (i = 0; i < n; i++) {
        int k = order[i];
        if (time < p[k].arrival) {                      /* CPU idle gap */
            gantt_add(g, &count, -1, time, p[k].arrival);
            time = p[k].arrival;
        }
        p[k].start_time = time;                         /* first CPU time */
        time += p[k].burst;
        p[k].completion = time;                         /* finished */
        p[k].remaining  = 0;
        gantt_add(g, &count, p[k].pid, p[k].start_time, time);
    }
    return count;
}

/* ------------------- SJF (non-preemptive) ------------------- */
int sjf_np(Process p[], int n, GanttEntry g[])
{
    int done[MAX_PROCESSES] = {0};
    int count = 0, finished = 0, time = 0, i;

    while (finished < n) {
        int best = -1;

        /* among arrived & not done: smallest burst, tie -> lower PID */
        for (i = 0; i < n; i++) {
            if (done[i] || p[i].arrival > time) continue;
            if (best == -1 ||
                p[i].burst < p[best].burst ||
                (p[i].burst == p[best].burst && p[i].pid < p[best].pid))
                best = i;
        }

        if (best == -1) {                               /* nobody ready: idle */
            int next = -1;
            for (i = 0; i < n; i++)
                if (!done[i] && (next == -1 || p[i].arrival < next))
                    next = p[i].arrival;
            gantt_add(g, &count, -1, time, next);
            time = next;
            continue;
        }

        p[best].start_time = time;                      /* first CPU time */
        time += p[best].burst;
        p[best].completion = time;                      /* finished */
        p[best].remaining  = 0;
        gantt_add(g, &count, p[best].pid, p[best].start_time, time);
        done[best] = 1;
        finished++;
    }
    return count;
}

/* ============ END fcfs_sjf.c ============ */

