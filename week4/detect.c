c
#include <string.h>
#include "deadlock.h"

static int leq(const int *a, const int *b, int m) {
    for (int j = 0; j < m; j++) if (a[j] > b[j]) return 0;
    return 1;
}

/* ---------- Deadlock detection (uses Allocation + Request) ---------- */
int detect_deadlock(System *s, int dead[]) {
    int work[MAXR], fin[MAXP], changed = 1, dc = 0;
    memcpy(work, s->available, sizeof(work));
    for (int i = 0; i < s->n; i++) {       /* a process holding nothing can't be deadlocked */
        fin[i] = 1;
        for (int j = 0; j < s->m; j++) if (s->alloc[i][j]) fin[i] = 0;
    }
    while (changed) {
        changed = 0;
        for (int i = 0; i < s->n; i++)
            if (!fin[i] && leq(s->request[i], work, s->m)) {
                for (int j = 0; j < s->m; j++) work[j] += s->alloc[i][j];
                fin[i] = 1; changed = 1;
            }
    }
    for (int i = 0; i < s->n; i++) if (!fin[i]) dead[dc++] = i;
    return dc;
}

/* ---------- Recovery: terminate the deadlocked process holding most resources, repeat ---------- */
int recover(System *s, int victims[]) {
    int dead[MAXP], v = 0, dc;
    while ((dc = detect_deadlock(s, dead)) > 0) {
        int best = dead[0], bestsum = -1;
        for (int k = 0; k < dc; k++) {
            int sum = 0;
            for (int j = 0; j < s->m; j++) sum += s->alloc[dead[k]][j];
            if (sum > bestsum) { bestsum = sum; best = dead[k]; }
        }
        for (int j = 0; j < s->m; j++) {   /* release everything it holds */
            s->available[j] += s->alloc[best][j];
            s->alloc[best][j] = s->max[best][j] = s->need[best][j] = s->request[best][j] = 0;
        }
        victims[v++] = best;
    }
    return v;
}
