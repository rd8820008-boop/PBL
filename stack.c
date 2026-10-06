#include "stack.h"

void initStack(Stack *stack) {
    stack->top = -1;
}

int isStackEmpty(Stack *stack) {
    return stack->top == -1;
}

int isStackFull(Stack *stack) {
    return stack->top == STACK_SIZE - 1;
}

void push(Stack *stack, int value) {
    if (!isStackFull(stack)) {
        stack->data[++stack->top] = value;
    }
}

int pop(Stack *stack) {
    if (!isStackEmpty(stack)) {
        return stack->data[stack->top--];
    }

    return -1;
}

int peek(Stack *stack) {
    if (!isStackEmpty(stack)) {
        return stack->data[stack->top];
    }

    return -1;
}