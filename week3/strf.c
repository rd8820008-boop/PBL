/*
 * srtf_priority.c
 *
 * 1. srtf        - Shortest Remaining Time First (preemptive)
 * 2. priority_np - Priority scheduling, non-preemptive
 * 3. priority_p  - Priority scheduling, preemptive
 *
 * Lower priority number = higher priority.
 * Tie-breaking: the process that arrived earlier goes first.
 *
 * Each function fills in start_time and completion for every process,
 * writes the Gantt chart into g[], and returns the number of Gantt
 * entries. Waiting, turnaround and response times are NOT calculated
 * here; calculate_metrics() does that afterwards.
 */

#include <stdio.h>
#include "scheduler.h"

#ifndef IDLE
#define IDLE -1               /* pid used in the Gantt chart when the CPU is idle */
#endif


/* ================= helper functions ================= */

/* Get every process ready for a fresh run */
static void reset_processes(Process p[], int n)
{
    for (int i = 0; i < n; i++) {
        p[i].remaining  = p[i].burst;
        p[i].start_time = -1;
        p[i].completion = 0;
        p[i].waiting    = 0;
        p[i].turnaround = 0;
        p[i].response   = 0;
    }
}

/* Record that `pid` used the CPU for one time unit, [t, t+1).
 * If the same process was already running just before, we stretch the
 * last entry instead of adding a new one. */
static void add_slice(GanttEntry g[], int *count, int pid, int t)
{
    if (*count > 0 && g[*count - 1].pid == pid && g[*count - 1].end == t) {
        g[*count - 1].end = t + 1;
        return;
    }
    if (*count < MAX_GANTT) {
        g[*count].pid   = pid;
        g[*count].start = t;
        g[*count].end   = t + 1;
        (*count)++;
    }
}

/* Is process a a better pick than process b?
 * by_priority = 1 -> compare priority numbers
 * by_priority = 0 -> compare remaining time (for SRTF) */
static int is_better(const Process *a, const Process *b, int by_priority)
{
    int a_key = by_priority ? a->priority : a->remaining;
    int b_key = by_priority ? b->priority : b->remaining;

    if (a_key != b_key)
        return a_key < b_key;
    return a->arrival < b->arrival;   /* tie -> earlier arrival wins */
}

/* Find the best process among those that have arrived and still have
 * work left. Returns -1 if nobody is ready right now. */
static int pick_next(Process p[], int n, int time, int by_priority)
{
    int best = -1;

    for (int i = 0; i < n; i++) {
        if (p[i].arrival > time || p[i].remaining == 0)
            continue;                         /* not here yet, or already done */
        if (best == -1 || is_better(&p[i], &p[best], by_priority))
            best = i;
    }
    return best;
}

/* Run process `cur` for one time unit starting at `time`.
 * Remembers when it first touched the CPU and when it finishes. */
static void run_one_unit(Process p[], int cur, int time,
                         GanttEntry g[], int *count, int *finished)
{
    if (p[cur].start_time == -1)
        p[cur].start_time = time;             /* first time on the CPU */

    add_slice(g, count, p[cur].pid, time);
    p[cur].remaining--;

    if (p[cur].remaining == 0) {
        p[cur].completion = time + 1;
        (*finished)++;
    }
}


/* ================= the three scheduling algorithms ================= */

/*
 * SRTF (preemptive Shortest Job First).
 * Every time unit, pick the process with the least remaining time.
 * A newly arrived short process can kick out the running one.
 */
int srtf(Process p[], int n, GanttEntry g[])
{
    int time = 0, finished = 0, count = 0;

    reset_processes(p, n);

    while (finished < n) {
        int cur = pick_next(p, n, time, 0);

        if (cur == -1) {                      /* nobody has arrived, CPU idles */
            add_slice(g, &count, IDLE, time);
            time++;
            continue;
        }

        run_one_unit(p, cur, time, g, &count, &finished);
        time++;
    }

    return count;
}

/*
 * Priority scheduling, NON-preemptive.
 * When the CPU becomes free, pick the highest-priority process that
 * has arrived and let it run until it completely finishes, even if a
 * more important process shows up in the meantime.
 */
int priority_np(Process p[], int n, GanttEntry g[])
{
    int time = 0, finished = 0, count = 0;

    reset_processes(p, n);

    while (finished < n) {
        int cur = pick_next(p, n, time, 1);

        if (cur == -1) {
            add_slice(g, &count, IDLE, time);
            time++;
            continue;
        }

        /* run the chosen process all the way through */
        while (p[cur].remaining > 0) {
            run_one_unit(p, cur, time, g, &count, &finished);
            time++;
        }
    }

    return count;
}

/*
 * Priority scheduling, PREEMPTIVE.
 * Same as priority_np, but we re-check every time unit. If a
 * higher-priority process arrives, it takes the CPU immediately and
 * the older one waits with whatever time it has left.
 */
int priority_p(Process p[], int n, GanttEntry g[])
{
    int time = 0, finished = 0, count = 0;

    reset_processes(p, n);

    while (finished < n) {
        int cur = pick_next(p, n, time, 1);

        if (cur == -1) {
            add_slice(g, &count, IDLE, time);
            time++;
            continue;
        }

        run_one_unit(p, cur, time, g, &count, &finished);
        time++;
    }

    return count;
}
