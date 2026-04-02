#include <pthread.h>
#include <string.h>
#include "encoder.h"
#include "protocol.h"

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  can_resume_cond = PTHREAD_COND_INITIALIZER;

// ==========================================
// INTERNAL HELPERS
// ==========================================

static void push_to_queue(GenericQueue* queue, Frame* frame) {
    pthread_mutex_lock(&queue_mutex);
    while (queue->count >= queue->max_capacity) {
        pthread_cond_wait(&can_resume_cond, &queue_mutex);
    }
    queue_push(queue, frame);
    pthread_mutex_unlock(&queue_mutex);
}

static void bytes_to_frame(const uint8_t b0, const uint8_t b1, const uint8_t b2, Frame* out) {
    uint32_t shift_register = ((uint32_t)b0 << 16) | ((uint32_t)b1 << 8) | b2;
    for (int c = 0; c < CELLS_PER_FRAME; c++) {
        out->cells[c] = (shift_register >> (21 - (c * 3))) & 0x07;
    }
}

static bool frame_equals_bytes(const Frame* frame, const uint8_t* bytes) {
    Frame candidate;
    bytes_to_frame(bytes[0], bytes[1], bytes[2], &candidate);
    return memcmp(frame->cells, candidate.cells, CELLS_PER_FRAME) == 0;
}


// Encodes a data frame and pushes it, handling escape and repeat logic.
static void push_data_frame(GenericQueue* queue, const uint8_t b0, const uint8_t b1, const uint8_t b2, Frame* last_pushed) {
    Frame f, f2;
    bytes_to_frame(b0, b1, b2, &f);

    // ---- Escape check ----
    if (frame_equals_bytes(&f, FRAME_START)  ||
        frame_equals_bytes(&f, FRAME_END)    ||
        frame_equals_bytes(&f, FRAME_REPEAT) ||
        frame_equals_bytes(&f, FRAME_ESCAPE))
    {
        // Push FRAME_ESCAPE as prefix
        bytes_to_frame(FRAME_ESCAPE[0], FRAME_ESCAPE[1], FRAME_ESCAPE[2], &f2);
        push_to_queue(queue, &f2);

        if (frame_equals_bytes(&f, FRAME_ESCAPE)) {
            // Special case: data IS escape — use TYPE2 as stand-in
            bytes_to_frame(FRAME_TYPE2_ESCAPE[0], FRAME_TYPE2_ESCAPE[1], FRAME_TYPE2_ESCAPE[2], &f);
        }
    }

    // ---- No escape needed: repeat check ----
    else if (!frame_equals_bytes(last_pushed, FRAME_REPEAT) && memcmp(f.cells, last_pushed->cells, CELLS_PER_FRAME) == 0) {
        bytes_to_frame(FRAME_REPEAT[0], FRAME_REPEAT[1], FRAME_REPEAT[2], &f);
    }

    push_to_queue(queue, &f);
    *last_pushed = f;
}


// ==========================================
// PUBLIC API
// ==========================================

uint8_t encode_payload_to_queue(const uint8_t* data, size_t data_length, GenericQueue* queue) {
    if (!data || !queue) return ERR_NULL_PTR;

    // last_pushed tracks the previous frame for the no-repeat rule.
    // We seed it with START since that's the first thing we push,
    // and it's guaranteed to differ from the length header bytes.
    Frame last_pushed;
    bytes_to_frame(FRAME_START[0], FRAME_START[1], FRAME_START[2], &last_pushed);

    // ---- 1. START ----
    push_to_queue(queue, &last_pushed);

    // ---- 2. LENGTH HEADER (3 bytes, 1 frame) ----
    uint8_t len_b0 = (data_length >> 16) & 0xFF;
    uint8_t len_b1 = (data_length >> 8)  & 0xFF;
    uint8_t len_b2 =  data_length        & 0xFF;
    push_data_frame(queue, len_b0, len_b1, len_b2, &last_pushed);

    // ---- 3. DATA FRAMES ----
    size_t full_chunks    = data_length / 3;
    size_t remainder_bytes = data_length % 3;
    size_t i = 0;

    for (size_t chunk = 0; chunk < full_chunks; chunk++, i += 3) {
        push_data_frame(queue, data[i], data[i+1], data[i+2], &last_pushed);
    }

    // Tail: 1 or 2 leftover bytes, zero-padded
    if (remainder_bytes > 0) {
        uint8_t b0 = data[i];
        uint8_t b1 = (remainder_bytes == 2) ? data[i+1] : 0x00;
        push_data_frame(queue, b0, b1, 0x00, &last_pushed);
    }

    // ---- 4. END ----
    Frame end_frame;
    if (frame_equals_bytes(&last_pushed, FRAME_END)) {
        bytes_to_frame(FRAME_REPEAT[0], FRAME_REPEAT[1], FRAME_REPEAT[2], &end_frame);
    }
    else{
        bytes_to_frame(FRAME_END[0], FRAME_END[1], FRAME_END[2], &end_frame);
    }
    push_to_queue(queue, &end_frame);

    return SUCCESS;
}