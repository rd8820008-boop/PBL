
#include <stdio.h>
#include "scheduler.h"

void print_gantt(GanttEntry g[], int count)
{
    if (count <= 0) {
        printf("\nNo Gantt chart data available.\n");
        return;
    }

    printf("\n========== GANTT CHART ==========\n");

    // Print process blocks
    for (int i = 0; i < count; i++) {
        if (g[i].pid == -1)
            printf("| IDLE ");
        else
            printf("| P%d ", g[i].pid);
    }
    printf("|\n");

    // Print start and end times
    printf("%d", g[0].start);

    for (int i = 0; i < count; i++) {
        printf("\t%d", g[i].end);
    }

    printf("\n=================================\n");
}
