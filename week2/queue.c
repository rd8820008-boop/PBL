#include "queue.h"

void initQueue(Queue *queue) {
    queue->front = 0;
    queue->rear = -1;
}

int isQueueEmpty(Queue *queue) {
    return queue->rear < queue->front;
}

int isQueueFull(Queue *queue) {
    return queue->rear == QUEUE_SIZE - 1;
}

void enqueue(Queue *queue, int value) {
    if (!isQueueFull(queue)) {
        queue->data[++queue->rear] = value;
    }
}

int dequeue(Queue *queue) {
    if (!isQueueEmpty(queue)) {
        return queue->data[queue->front++];
    }

    return -1;
}

int peekQueue(Queue *queue) {
    if (!isQueueEmpty(queue)) {
        return queue->data[queue->front];
    }

    return -1;
}