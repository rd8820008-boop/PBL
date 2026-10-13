#ifndef CPU_H
#define CPU_H

typedef struct {
    int registers[4];
    int programCounter;
} CPU;

void initCPU(CPU *cpu);

int executeOperation(CPU *cpu, int operation, int a, int b);

#endif