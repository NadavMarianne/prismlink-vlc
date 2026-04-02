#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "generic_queue.h"

// Custom struct mimicking a network packet for PrismLink
typedef struct {
    int packet_id;
    int payload_size;
    bool is_encrypted;
} NetworkPacket;

int main() {
    printf("--- Running Ultimate Generic Queue Tests ---\n");

    GenericQueue q;
    int buffer[3]; // Small capacity forces edge cases faster
    int capacity = 3;
    
    // ==========================================
    // PHASE 1: Initialization & Empty State
    // ==========================================
    initQueue(&q, buffer, capacity, sizeof(int));
    assert(isEmpty(&q) == true);
    assert(isFull(&q) == false);
    
    // Test Underflow Rejection
    int garbage = 0;
    assert(dequeue(&q, &garbage) == false); // Should reject dequeueing an empty queue
    
    printf("[PASS] Initialization & Underflow rejection.\n");


    // ==========================================
    // PHASE 2: Linear Fill & Overflow State
    // ==========================================
    int a = 10, b = 20, c = 30, d = 40;
    
    assert(enqueue(&q, &a) == true);
    assert(isEmpty(&q) == false); // No longer empty
    
    assert(enqueue(&q, &b) == true);
    assert(enqueue(&q, &c) == true);
    
    // The queue should now be exactly full
    assert(isFull(&q) == true);
    
    // Test Overflow Rejection
    assert(enqueue(&q, &d) == false); // Should reject the 4th item
    
    printf("[PASS] Capacity limits & Overflow rejection.\n");


    // ==========================================
    // PHASE 3: The "Ring Buffer" Wrap-Around Test
    // ==========================================
    // Right now, the buffer is [10, 20, 30]. Both front and rear are likely at the end.
    
    int out_item;
    
    // 1. Dequeue the first item (10)
    assert(dequeue(&q, &out_item) == true);
    assert(out_item == 10);
    assert(isFull(&q) == false); // It has room for 1 more now
    
    // 2. Enqueue a new item (40). 
    // If the queue is a proper ring buffer, this should wrap around to index 0!
    assert(enqueue(&q, &d) == true);
    assert(isFull(&q) == true); // Full again
    
    // 3. Drain the queue completely to verify order: Should be 20, 30, 40.
    assert(dequeue(&q, &out_item) == true); assert(out_item == 20);
    assert(dequeue(&q, &out_item) == true); assert(out_item == 30);
    assert(dequeue(&q, &out_item) == true); assert(out_item == 40);
    
    assert(isEmpty(&q) == true); // Should be completely empty again
    
    printf("[PASS] Ring buffer wrap-around & FIFO integrity.\n");


    // ==========================================
    // PHASE 4: Complex Type Integration (Structs)
    // ==========================================
    GenericQueue packet_q;
    NetworkPacket p_buffer[2]; 
    
    initQueue(&packet_q, p_buffer, 2, sizeof(NetworkPacket));
    
    NetworkPacket p_in1 = { .packet_id = 101, .payload_size = 512, .is_encrypted = true };
    NetworkPacket p_in2 = { .packet_id = 102, .payload_size = 256, .is_encrypted = false };
    
    assert(enqueue(&packet_q, &p_in1) == true);
    assert(enqueue(&packet_q, &p_in2) == true);
    
    NetworkPacket p_out;
    
    assert(dequeue(&packet_q, &p_out) == true);
    assert(p_out.packet_id == 101);
    assert(p_out.payload_size == 512);
    assert(p_out.is_encrypted == true);
    
    assert(dequeue(&packet_q, &p_out) == true);
    assert(p_out.packet_id == 102);
    
    printf("[PASS] Dynamic byte-size memory copying (Structs).\n");

    // ==========================================
    printf("\n>>> ALL GENERIC QUEUE TESTS PASSED <<<\n");
    return 0;
}