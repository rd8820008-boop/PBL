#ifndef STACK_H
#define STACK_H

#define STACK_SIZE 100

typedef struct {
    int data[STACK_SIZE];
    int top;
} Stack;

void initStack(Stack *stack);
int isStackEmpty(Stack *stack);
int isStackFull(Stack *stack);
void push(Stack *stack, int value);
int pop(Stack *stack);
int peek(Stack *stack);

#endif