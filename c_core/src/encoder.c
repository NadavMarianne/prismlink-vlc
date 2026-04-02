#include <pthread.h>
#include "encoder.h"

// The Global "Flag" system
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  can_resume_cond = PTHREAD_COND_INITIALIZER;

static void push_to_queue(GenericQueue* queue, Frame* frame) {
    pthread_mutex_lock(&queue_mutex);

    // If full, we wait for Android to "ring the bell"
    while (queue->count >= queue->max_capacity) {
        // This is the "Flag". The thread stops here and uses 0% CPU.
        pthread_cond_wait(&can_resume_cond, &queue_mutex);
    }

    // Push the frame
    queue_push(queue, frame);

    pthread_mutex_unlock(&queue_mutex);
}


uint8_t encode_payload_to_queue(const uint8_t* data, size_t data_length, GenericQueue* queue) {
    if (!data || !queue) return ERR_NULL_PTR;

    // Calculate our exact loop boundaries
    size_t full_chunks = data_length / 3;
    size_t remainder_bytes = data_length % 3;
    size_t bytes_processed = 0;
    Frame current_frame;

    // ==========================================
    // 1. THE FAST PATH (No if-statements)
    // ==========================================
    // We KNOW every iteration here has exactly 3 bytes.
    for (size_t i = 0; i < full_chunks; i++) {
        
        uint32_t shift_register = 0;
        
        // Scoop exactly 3 bytes directly. No bounds checking.
        shift_register |= (data[bytes_processed++] << 16);
        shift_register |= (data[bytes_processed++] << 8);
        shift_register |= (data[bytes_processed++]);

        // Slice into 8 cells
        for (int c = 0; c < CELLS_PER_FRAME; c++) {
            current_frame.cells[c] = (shift_register >> (21 - (c * 3))) & 0x07;
        }

        // Push to Android queue
        push_to_queue(queue, &current_frame);
    }

    // ==========================================
    // 2. THE TAIL PATH (The Leftovers)
    // ==========================================
    // Handle the last 1 or 2 bytes if the data wasn't perfectly divisible by 3.
    if (remainder_bytes > 0) {
        uint32_t shift_register = 0;
        // We know we have at least 1 byte left
        shift_register |= (data[bytes_processed++] << 16);
        // Load the 2nd leftover byte if it exists
        if (remainder_bytes == 2) {
            shift_register |= (data[bytes_processed++] << 8);
        }

        // Slice into 8 cells (the empty space automatically becomes 0s / Black)
        for (int c = 0; c < CELLS_PER_FRAME; c++) {
            current_frame.cells[c] = (shift_register >> (21 - (c * 3))) & 0x07;
        }

        // Push to Android queue
        push_to_queue(queue, &current_frame);
    }
    
    return SUCCESS;
}