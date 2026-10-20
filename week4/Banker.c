#include <string.h>
#include "deadlock.h"

static int leq(const int *a, const int *b, int m) {
    for (int j = 0; j < m; j++) if (a[j] > b[j]) return 0;
    return 1;
}

void calc_need(System *s) {
    for (int i = 0; i < s->n; i++)
        for (int j = 0; j < s->m; j++)
            s->need[i][j] = s->max[i][j] - s->alloc[i][j];
}

/* ---------- Banker's safety algorithm ---------- */
int is_safe(System *s, int seq[]) {
    int work[MAXR], fin[MAXP] = {0}, cnt = 0;
    memcpy(work, s->available, sizeof(work));
    while (cnt < s->n) {
        int found = 0;
        for (int i = 0; i < s->n && !found; i++) {
            if (!fin[i] && leq(s->need[i], work, s->m)) {
                for (int j = 0; j < s->m; j++) work[j] += s->alloc[i][j];
                fin[i] = 1; seq[cnt++] = i; found = 1;
            }
        }
        if (!found) return 0;          /* no process can finish -> UNSAFE */
    }
    return 1;
}

/* ---------- Banker's resource-request algorithm ---------- */
int request_resources(System *s, int p, int req[], char *msg) {
    int seq[MAXP];
    if (p < 0 || p >= s->n) { strcpy(msg, "Error: invalid process"); return -1; }
    if (!leq(req, s->need[p], s->m)) {
        strcpy(msg, "Error: request exceeds the process's maximum claim (Need)");
        return -1;
    }n/ 
    if (!leq(req, s->available, s->m)) {
        strcpy(msg, "Wait: not enough resources available right now");
        return 0;
    }
    for (int j = 0; j < s->m; j++) {       /* pretend to allocate */
        s->available[j] -= req[j]; s->alloc[p][j] += req[j]; s->need[p][j] -= req[j];
    }
    if (is_safe(s, seq)) { strcpy(msg, "Granted: system stays in a SAFE state"); return 1; }
    for (int j = 0; j < s->m; j++) {       /* roll back */
        s->available[j] += req[j]; s->alloc[p][j] -= req[j]; s->need[p][j] += req[j];
    }
    strcpy(msg, "Denied: granting would lead to an UNSAFE state");
    return 0;
}
