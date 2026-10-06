#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_SIZE 100

typedef struct {
    int data[QUEUE_SIZE];
    int front;
    int rear;
} Queue;

void initQueue(Queue *queue);
int isQueueEmpty(Queue *queue);
int isQueueFull(Queue *queue);
void enqueue(Queue *queue, int value);
int dequeue(Queue *queue);
int peekQueue(Queue *queue);

#endif