#include <stdio.h>
#include <string.h> // Required for memcpy
#include "generic_queue.h"

void initQueue(GenericQueue* q, void* buffer, int capacity, int item_size) {
    q->data = buffer;
    q->max_capacity = capacity;
    q->item_size = item_size;
    q->front = -1;
    q->rear = -1;
    q->count = 0;
}

bool isFull(GenericQueue* q) {
    return q->count == q->max_capacity;
}

bool isEmpty(GenericQueue* q) {
    return q->count == 0;
}

bool enqueue(GenericQueue* q, const void* item_ptr) {
    if (isFull(q)) {
        printf("Queue Overflow!\n");
        return false;
    }

    void* target_address = (char*)q->data + (q->rear * q->item_size);
    memcpy(target_address, item_ptr, q->item_size);

    q->rear = (q->rear + 1) % q->max_capacity;
    q->count++;
    return true;
}

bool dequeue(GenericQueue* q, void* dest_ptr) {
    if (isEmpty(q)) {
        printf("Queue Underflow!\n");
        return false;
    }

    void* source_address = (char*)q->data + (q->front * q->item_size);
    memcpy(dest_ptr, source_address, q->item_size);

    q->front = (q->front + 1) % q->max_capacity;
    q->count--;
    return true;
}