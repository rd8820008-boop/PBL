#ifndef DEADLOCK_H
#define DEADLOCK_H

#define MAXP 10   /* max processes */
#define MAXR 10   /* max resource types */

typedef struct {
    int n, m;                    /* processes, resource types */
    int available[MAXR];         /* Available vector */
    int alloc[MAXP][MAXR];       /* Allocation matrix */
    int max[MAXP][MAXR];         /* Maximum matrix */
    int need[MAXP][MAXR];        /* Need = Max - Alloc */
    int request[MAXP][MAXR];     /* pending (waiting) requests */
} System;

void calc_need(System *s);
/* Banker's safety algorithm: returns 1 if SAFE (fills seq), 0 if UNSAFE */
int  is_safe(System *s, int seq[]);
/* Banker's resource request: 1 granted, 0 wait/denied, -1 error. Writes reason in msg */
int  request_resources(System *s, int p, int req[], char *msg);
/* Deadlock detection: returns number of deadlocked processes, fills dead[] */
int  detect_deadlock(System *s, int dead[]);
/* Recovery by process termination: returns number of victims, fills victims[] */
int  recover(System *s, int victims[]);

#endif