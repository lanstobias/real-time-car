/**
 * @file Queue.c
 *
 * @author Tobias Lans
 * @date 2018-05-07
 **/
 
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include "Queue.h"

int queueInit(queue* self, int size)
{
    assert(self != NULL);
    if (size < 1)
    {
        return EXIT_FAILURE;
    }

    self->size = size;
    self->nrOfElements = 0;
    self->array = (int*)malloc(self->size * sizeof(int));
    self->front = 0;
    self->rear = -1;

    if (self->array == NULL)
    {
        printf("Coundn't create the queue: %s", strerror(errno));
        return false;
    }

    return EXIT_SUCCESS;
}

int queueDequeue(queue* self)
{
    int dequeuedValue;

    if (queueIsEmpty(self))
    {
        return EXIT_FAILURE;
    }

    dequeuedValue = self->array[self->front];
    self->front = ( (self->front + 1) % self->size );
    self->nrOfElements--;

    return dequeuedValue;
}

int queueEnqueue(queue* self, int value)
{
    if (queueIsFull(self))
    {
        return EXIT_FAILURE;
    }

    queue_add(self, value);
    self->nrOfElements++;

    return EXIT_SUCCESS;
}

void queueClear(queue* self)
{
    assert(self != NULL);

    self->nrOfElements = 0;
    self->front = 0;
    self->rear = -1;
}

int queueIsFull(queue* self)
{
    assert(self != NULL);

    if (self->rear == -1)
    {
        return false;
    }
    
    int returnVal = ( ((self->rear + 1) % self->size) == self->front);
    return returnVal;
}

int queueIsEmpty(queue* self)
{
    assert(self != NULL);

    return !queueSize(self);
}

int queueSize(queue* self)
{
    assert(self != NULL);

    return self->nrOfElements;
}

int queueFirst(queue* self)
{
    assert(self != NULL);

    if (queueIsEmpty(self))
    {
        return 0;
    }

    return self->array[(self->front % self->size)];
}

int queueLast(queue* self)
{
    assert(self != NULL);

    if (queueIsEmpty(self))
    {
        return 0;
    }

    return self->array[( (self->rear) % self->size )];
}

void queue_add(queue* self, int value)
{
    self->rear = ((self->rear + 1) % self->size);
    self->array[self->rear] = value;
}

void queueDestroy(queue* self)
{
    free(self->array);
    self = NULL;
}