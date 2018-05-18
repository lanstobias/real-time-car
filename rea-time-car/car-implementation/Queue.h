/**
 * @file Queue.h
 *
 * @author Tobias Lans
 * @date 2018-05-07
 **/
 
#ifndef QUEUE_H
#define QUEUE_H

#define MAX_SIZE 10000

typedef struct
{
    int rear, front;
    int* array;
    int size;
    int nrOfElements;
} queue;

// Constructor
int queueInit(queue* self, int size);

// Methods
int queueDequeue(queue* self);
int queueEnqueue(queue* self, int value);
void queueClear(queue* self);
int queueIsFull(queue* self);
int queueIsEmpty(queue* self);
int queueSize(queue* self);
int queueFirst(queue* self);
int queueLast(queue* self);

int queue_create(queue* self);
void queue_add(queue* self, int value);
void queueDestroy(queue* self);

#endif