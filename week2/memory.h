#ifndef MEMORY_H
#define MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

int *allocateMemory(int size);
void writeMemory(int *memory, int index, int value);
int readMemory(int *memory, int index);
void freeMemory(int *memory);

#ifdef __cplusplus
}
#endif

#endif