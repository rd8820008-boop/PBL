#include <stdio.h>

#include "cpu.h"
#include "memory.h"
#include "stack.h"
#include "queue.h"

int main() {

    printf("=== Core Process Started ===\n");

    CPU cpu;
    initCPU(&cpu);

    int *memory = allocateMemory(10);

    Stack stack;
    initStack(&stack);

    Queue queue;
    initQueue(&queue);

    int choice;

    while (1) {

        printf("\n--- Core Menu ---\n");
        printf("1. Execute CPU instruction\n");
        printf("2. Write to Memory\n");
        printf("3. Read Memory\n");
        printf("4. Push to Stack\n");
        printf("5. Pop from Stack\n");
        printf("6. Enqueue\n");
        printf("7. Dequeue\n");
        printf("0. Exit\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 0) {
            break;
        }

       if (choice == 1) {

    int operation;
    int a, b;
    int result;

    printf("\n--- CPU Operations ---\n");
    printf("1. Add\n");
    printf("2. Subtract\n");
    printf("3. Multiply\n");
    printf("4. Divide\n");

    printf("Enter operation: ");
    scanf("%d", &operation);

    printf("Enter first number: ");
    scanf("%d", &a);

    printf("Enter second number: ");
    scanf("%d", &b);

    result = executeOperation(&cpu, operation, a, b);

    printf("Result: %d\n", result);
    printf("CPU Register 0: %d\n", cpu.registers[0]);
    printf("Program Counter: %d\n", cpu.programCounter);
}
        else if (choice == 2) {
            int index, value;

            printf("Enter memory index: ");
            scanf("%d", &index);

            printf("Enter value: ");
            scanf("%d", &value);

            writeMemory(memory, index, value);

            printf("Memory updated.\n");
        }

        else if (choice == 3) {
            int index;

            printf("Enter memory index: ");
            scanf("%d", &index);

            printf("Memory[%d] = %d\n",
                   index,
                   readMemory(memory, index));
        }

        else if (choice == 4) {
            int value;

            printf("Enter value: ");
            scanf("%d", &value);

            push(&stack, value);

            printf("Value pushed to stack.\n");
        }

        else if (choice == 5) {
            printf("Popped value: %d\n", pop(&stack));
        }

        else if (choice == 6) {
            int value;

            printf("Enter value: ");
            scanf("%d", &value);

            enqueue(&queue, value);

            printf("Value added to queue.\n");
        }

        else if (choice == 7) {
            printf("Dequeued value: %d\n", dequeue(&queue));
        }

        else {
            printf("Invalid choice.\n");
        }
    }

    freeMemory(memory);

    printf("\n=== Core Process Finished ===\n");

    return 0;
}