#ifndef GENERIC_QUEUE_H
#define GENERIC_QUEUE_H

#include <stdbool.h> // For boolean return types

typedef struct {
    void* data;         // Pointer to the external flat byte array
    int item_size;      // Size of a single element (in bytes)
    int max_capacity;   // Maximum number of elements
    int front;
    int rear;
} GenericQueue;

// Prototypes
void initQueue(GenericQueue* q, void* buffer, int capacity, int item_size);
bool isFull(GenericQueue* q);
bool isEmpty(GenericQueue* q);

// Notice these take void pointers. 'enqueue' reads from item_ptr, 'dequeue' writes to dest_ptr.
bool enqueue(GenericQueue* q, const void* item_ptr);
bool dequeue(GenericQueue* q, void* dest_ptr);

#endif // GENERIC_QUEUE_H