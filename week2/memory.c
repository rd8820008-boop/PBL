#include <stdlib.h>
#include "memory.h"

int *allocateMemory(int size) {
    return (int *)malloc(size * sizeof(int));
}

void writeMemory(int *memory, int index, int value) {
    memory[index] = value;
}

int readMemory(int *memory, int index) {
    return memory[index];
}

void freeMemory(int *memory) {
    free(memory);
}
