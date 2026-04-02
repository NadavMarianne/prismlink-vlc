#include "decoder.h"
#include <pthread.h>

// Assuming these are the same globals from encoder.c
extern pthread_mutex_t queue_mutex;
extern pthread_cond_t data_ready_cond; 
extern pthread_cond_t can_resume_cond;

uint8_t decode_queue_to_payload(GenericQueue* queue, uint8_t* out_data, size_t expected_length) {
    if (!queue || !out_data) return ERR_NULL_PTR;

    size_t bytes_written = 0;
    Frame current_frame;

    while (bytes_written < expected_length) {
        // 1. POP from queue (Thread Safe)
        pthread_mutex_lock(&queue_mutex);
        
        while (queue->count == 0) {
            // Wait for Android Camera to push something
            pthread_cond_wait(&data_ready_cond, &queue_mutex);
        }

        dequeue(queue, &current_frame);

        // If we just hit the 20% mark, tell Android to "Burst" more in
        if (queue->count == (queue->max_capacity * 0.2)) {
            pthread_cond_signal(&can_resume_cond);
        }
        
        pthread_mutex_unlock(&queue_mutex);

        // 2. RECONSTRUCT the 24-bit register
        uint32_t shift_register = 0;
        for (int c = 0; c < CELLS_PER_FRAME; c++) {
            shift_register |= ((uint32_t)(current_frame.cells[c] & 0x07) << (21 - (c * 3)));
        }

        // 3. EXTRACT bytes (handles Tail Path automatically)
        out_data[bytes_written++] = (shift_register >> 16) & 0xFF;
        
        if (bytes_written < expected_length) {
            out_data[bytes_written++] = (shift_register >> 8) & 0xFF;
        }
        if (bytes_written < expected_length) {
            out_data[bytes_written++] = (shift_register) & 0xFF;
        }
    }

    return SUCCESS;
}