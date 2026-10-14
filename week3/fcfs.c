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

/* ================= BEGIN scheduler.h (pasted inline) ================= */
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
/* ================== END scheduler.h (pasted inline) ================== */

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

/* ================= test helpers + main (standalone only) ================= */

#define CELL 6   /* width of one Gantt block (characters between the bars) */

static void gantt_border(int count)
{
    int i, k;
    for (i = 0; i < count; i++) {
        printf("+");
        for (k = 0; k < CELL; k++) printf("-");
    }
    printf("+\n");
}

static void show_gantt(GanttEntry g[], int count)
{
    int i, len, left, right;
    char label[16];

    printf("Gantt Chart:\n");
    gantt_border(count);

    /* process labels, centered in each block */
    for (i = 0; i < count; i++) {
        if (g[i].pid == -1) snprintf(label, sizeof label, "IDLE");
        else                snprintf(label, sizeof label, "P%d", g[i].pid);
        len   = (int)strlen(label);
        left  = (CELL - len) / 2;
        right = CELL - len - left;
        printf("|%*s%s%*s", left, "", label, right, "");
    }
    printf("|\n");

    gantt_border(count);

    /* time markers, aligned under each border */
    for (i = 0; i < count; i++)
        printf("%-*d", CELL + 1, g[i].start);
    if (count) printf("%d", g[count - 1].end);
    printf("\n");
}

static void show_metrics(Process p[], int n)
{
    int i;
    double wt = 0, tat = 0;
    printf("PID  AT  BT  ST  CT  WT  TAT  RT\n");
    for (i = 0; i < n; i++) {
        p[i].turnaround = p[i].completion - p[i].arrival;
        p[i].waiting    = p[i].turnaround - p[i].burst;
        p[i].response   = p[i].start_time - p[i].arrival;
        wt  += p[i].waiting;
        tat += p[i].turnaround;
        printf("P%-3d %2d  %2d  %2d  %2d  %2d  %3d  %2d\n", p[i].pid,
               p[i].arrival, p[i].burst, p[i].start_time, p[i].completion,
               p[i].waiting, p[i].turnaround, p[i].response);
    }
    printf("Avg WT = %.2f   Avg TAT = %.2f\n\n", wt / n, tat / n);
}

static void run_case(const char *title, int n, int at[], int bt[],
                     int (*algo)(Process[], int, GanttEntry[]), const char *name)
{
    Process p[MAX_PROCESSES];
    GanttEntry g[MAX_GANTT];
    int i, count;

    for (i = 0; i < n; i++) {
        p[i].pid = i + 1;       p[i].arrival = at[i];
        p[i].burst = bt[i];     p[i].priority = 0;
        p[i].remaining = bt[i]; p[i].start_time = -1;
        p[i].completion = p[i].waiting = p[i].turnaround = p[i].response = 0;
    }
    printf("=== %s : %s ===\n", title, name);
    count = algo(p, n, g);
    show_gantt(g, count);
    show_metrics(p, n);
}

int main(void)
{
    /* Test 1: classic case */
    int at1[] = {0, 2, 4, 5}, bt1[] = {7, 4, 1, 4};
    /* Test 2: all arrive together */
    int at2[] = {0, 0, 0},    bt2[] = {6, 2, 4};
    /* Test 3: idle gap in the middle */
    int at3[] = {0, 2, 20},   bt3[] = {5, 3, 3};

    run_case("Test 1", 4, at1, bt1, fcfs,   "FCFS");
    run_case("Test 1", 4, at1, bt1, sjf_np, "SJF (non-preemptive)");
    run_case("Test 2", 3, at2, bt2, fcfs,   "FCFS");
    run_case("Test 2", 3, at2, bt2, sjf_np, "SJF (non-preemptive)");
    run_case("Test 3", 3, at3, bt3, fcfs,   "FCFS");
    run_case("Test 3", 3, at3, bt3, sjf_np, "SJF (non-preemptive)");
    return 0;
}
