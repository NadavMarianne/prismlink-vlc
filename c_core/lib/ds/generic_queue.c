#include <stdio.h>
#include <string.h> // Required for memcpy
#include "ds/generic_queue.h"

void initQueue(GenericQueue* q, void* buffer, int capacity, int item_size) {
    q->data = buffer;
    q->max_capacity = capacity;
    q->item_size = item_size;
    q->front = -1;
    q->rear = -1;
}

bool isFull(GenericQueue* q) {
    return (q->rear + 1) % q->max_capacity == q->front;
}

bool isEmpty(GenericQueue* q) {
    return q->front == -1;
}

bool enqueue(GenericQueue* q, const void* item_ptr) {
    if (isFull(q)) {
        printf("Queue Overflow!\n");
        return false;
    }
    
    if (q->front == -1) {
        q->front = 0;
    }
    
    q->rear = (q->rear + 1) % q->max_capacity;
    
    // Calculate the exact memory address in the byte buffer
    void* target_address = (char*)q->data + (q->rear * q->item_size);
    
    // Copy the raw bytes from the user's item into the queue
    memcpy(target_address, item_ptr, q->item_size);
    return true;
}

bool dequeue(GenericQueue* q, void* dest_ptr) {
    if (isEmpty(q)) {
        printf("Queue Underflow!\n");
        return false;
    }
    
    // Calculate the memory address of the item at the front
    void* source_address = (char*)q->data + (q->front * q->item_size);
    
    // Copy the raw bytes out of the queue and into the user's provided variable
    memcpy(dest_ptr, source_address, q->item_size);
    
    if (q->front == q->rear) {
        q->front = q->rear = -1; // Reset queue
    } else {
        q->front = (q->front + 1) % q->max_capacity;
    }
    return true;
}