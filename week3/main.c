#include <stdio.h>
#include "scheduler.h"

static Process sample[] = {
    {1, 0, 5, 2, 5, -1, 0, 0, 0, 0},
    {2, 1, 3, 1, 3, -1, 0, 0, 0, 0},
    {3, 2, 8, 4, 8, -1, 0, 0, 0, 0},
    {4, 3, 6, 3, 6, -1, 0, 0, 0, 0}
};

static void reset(Process dst[], Process src[], int n) {
    for (int i = 0; i < n; i++) {
        dst[i] = src[i];
        dst[i].remaining  = dst[i].burst;
        dst[i].start_time = -1;
        dst[i].completion = dst[i].waiting = 0;
        dst[i].turnaround = dst[i].response = 0;
    }
}

static void run(int choice, Process master[], int n, int quantum) {
    Process p[MAX_PROCESSES];
    GanttEntry g[MAX_GANTT];
    int cnt = 0;
    const char *names[] = {"", "FCFS", "SJF (non-preemptive)", "SRTF",
                           "Priority (non-preemptive)", "Priority (preemptive)",
                           "Round Robin"};
    reset(p, master, n);
    printf("\n===== %s =====\n", names[choice]);
    switch (choice) {
        case 1: cnt = fcfs(p, n, g); break;
        case 2: cnt = sjf_np(p, n, g); break;
        case 3: cnt = srtf(p, n, g); break;
        case 4: cnt = priority_np(p, n, g); break;
        case 5: cnt = priority_p(p, n, g); break;
        case 6: cnt = round_robin(p, n, quantum, g); break;
    }
    print_gantt(g, cnt);
    calculate_metrics(p, n);
    print_metrics(p, n);
}

static int read_processes(Process p[]) {
    int n;
    printf("Number of processes (1-%d): ", MAX_PROCESSES);
    if (scanf("%d", &n) != 1 || n < 1 || n > MAX_PROCESSES) return 0;
    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;
        printf("P%d  arrival burst priority: ", i + 1);
        if (scanf("%d %d %d", &p[i].arrival, &p[i].burst, &p[i].priority) != 3)
            return 0;
        p[i].remaining = p[i].burst;
        p[i].start_time = -1;
        p[i].completion = p[i].waiting = p[i].turnaround = p[i].response = 0;
    }
    return n;
}

int main(void) {
    Process master[MAX_PROCESSES];
    int n = 4, quantum = 2, choice;
    reset(master, sample, n);

    do {
        printf("\n===== PROCESS SCHEDULING SIMULATOR =====\n");
        printf("1. FCFS\n2. SJF (non-preemptive)\n3. SRTF\n");
        printf("4. Priority (non-preemptive)\n5. Priority (preemptive)\n");
        printf("6. Round Robin\n7. Run all algorithms\n");
        printf("8. Enter new processes\n9. Use sample data\n0. Exit\n");
        printf("Choice: ");
        if (scanf("%d", &choice) != 1) break;

        if (choice == 6 || choice == 7) {
            printf("Time quantum: ");
            if (scanf("%d", &quantum) != 1 || quantum < 1) quantum = 2;
        }
        if (choice >= 1 && choice <= 6) run(choice, master, n, quantum);
        else if (choice == 7) for (int c = 1; c <= 6; c++) run(c, master, n, quantum);
        else if (choice == 8) {
            int m = read_processes(master);
            if (m) n = m; else printf("Invalid input, keeping old data.\n");
        } else if (choice == 9) { n = 4; reset(master, sample, n); }
    } while (choice != 0);

    return 0;
}