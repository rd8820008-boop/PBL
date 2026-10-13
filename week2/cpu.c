#include "cpu.h"

void initCPU(CPU *cpu) {
    for (int i = 0; i < 4; i++) {
        cpu->registers[i] = 0;
    }

    cpu->programCounter = 0;
}

int executeOperation(CPU *cpu, int operation, int a, int b) {

    int result = 0;

    switch (operation) {

        case 1:
            result = a + b;
            break;

        case 2:
            result = a - b;
            break;

        case 3:
            result = a * b;
            break;

        case 4:
            if (b != 0) {
                result = a / b;
            } else {
                return 0;
            }
            break;

        default:
            return 0;
    }

    cpu->registers[0] = result;
    cpu->programCounter++;

    return result;
}