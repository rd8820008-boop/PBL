/* Reads a command + system state from stdin, prints the new state as JSON.
   Usage:  ./deadlock < input.txt     (first word = status | request | recover) */
#include <stdio.h>
#include <string.h>
#include "deadlock.h"

static void pvec(const int *v, int m) {
    printf("[");
    for (int j = 0; j < m; j++) printf("%s%d", j ? "," : "", v[j]);
    printf("]");
}
static void pmat(int a[][MAXR], int n, int m) {
    printf("[");
    for (int i = 0; i < n; i++) { if (i) printf(","); pvec(a[i], m); }
    printf("]");
}
static void out(System *s, const char *msg, int granted) {
    int seq[MAXP], dead[MAXP];
    int dc = detect_deadlock(s, dead);
    int safe = is_safe(s, seq);
    const char *st = dc ? "DEADLOCKED" : (safe ? "SAFE" : "UNSAFE");
    printf("{\"n\":%d,\"m\":%d,\"available\":", s->n, s->m); pvec(s->available, s->m);
    printf(",\"alloc\":");   pmat(s->alloc, s->n, s->m);
    printf(",\"max\":");     pmat(s->max, s->n, s->m);
    printf(",\"need\":");    pmat(s->need, s->n, s->m);
    printf(",\"request\":"); pmat(s->request, s->n, s->m);
    printf(",\"safe_seq\":"); pvec(seq, safe ? s->n : 0);
    printf(",\"deadlocked\":"); pvec(dead, dc);
    printf(",\"state\":\"%s\",\"granted\":%d,\"msg\":\"%s\"}\n", st, granted, msg);
}
static void fail(const char *e) { printf("{\"error\":\"%s\"}\n", e); }

int main(void) {
    char cmd[32]; static System s;
    if (scanf("%31s %d %d", cmd, &s.n, &s.m) != 3) { fail("bad input"); return 1; }
    if (s.n < 1 || s.n > MAXP || s.m < 1 || s.m > MAXR) { fail("processes/resources must be 1 to 10"); return 1; }
    for (int j = 0; j < s.m; j++) scanf("%d", &s.available[j]);
    for (int i = 0; i < s.n; i++) for (int j = 0; j < s.m; j++) scanf("%d", &s.alloc[i][j]);
    for (int i = 0; i < s.n; i++) for (int j = 0; j < s.m; j++) scanf("%d", &s.max[i][j]);
    for (int i = 0; i < s.n; i++) for (int j = 0; j < s.m; j++) scanf("%d", &s.request[i][j]);
    calc_need(&s);
    for (int i = 0; i < s.n; i++) for (int j = 0; j < s.m; j++)
        if (s.need[i][j] < 0) { fail("Allocation is greater than Maximum for some process"); return 1; }

    if (!strcmp(cmd, "status")) {
        out(&s, "Analysis complete", -2);
    } else if (!strcmp(cmd, "request")) {
        int p, req[MAXR]; char msg[128];
        scanf("%d", &p);
        for (int j = 0; j < s.m; j++) scanf("%d", &req[j]);
        int g = request_resources(&s, p, req, msg);
        out(&s, msg, g);
    } else if (!strcmp(cmd, "recover")) {
        int vic[MAXP], v = recover(&s, vic); char msg[128] = "No deadlock, nothing to recover";
        if (v) {
            strcpy(msg, "Recovered by terminating:");
            for (int k = 0; k < v; k++) sprintf(msg + strlen(msg), " P%d", vic[k]);
        }
        out(&s, msg, -2);
    } else fail("unknown command");
    return 0;
}