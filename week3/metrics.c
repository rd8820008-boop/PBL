#include <stdio.h>
#include "scheduler.h"

void calculate_metrics(Process p[], int n) {
    for (int i = 0; i < n; i++) {
        p[i].turnaround = p[i].completion - p[i].arrival;
        p[i].waiting    = p[i].turnaround - p[i].burst;
        p[i].response   = p[i].start_time - p[i].arrival;
    }
}

void print_metrics(Process p[], int n) {
    float tw = 0, tt = 0, tr = 0;
    printf("PID\tAT\tBT\tCT\tWT\tTAT\tRT\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\n", p[i].pid, p[i].arrival,
               p[i].burst, p[i].completion, p[i].waiting,
               p[i].turnaround, p[i].response);
        tw += p[i].waiting;
        tt += p[i].turnaround;
        tr += p[i].response;
    }
    printf("Average WT = %.2f, TAT = %.2f, RT = %.2f\n\n", tw / n, tt / n, tr / n);
}