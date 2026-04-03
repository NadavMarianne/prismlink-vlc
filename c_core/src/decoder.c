#include <string.h>
#include "decoder.h"
#include "protocol.h"

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  can_consume_cond = PTHREAD_COND_INITIALIZER;
// ==========================================
// INTERNAL HELPERS
// ==========================================

static void pop_from_queue(GenericQueue* queue, Frame* out) {
    pthread_mutex_lock(&queue_mutex);
    while (queue->count == 0) {
        pthread_cond_wait(&can_consume_cond, &queue_mutex);
    }
    queue_pop(queue, out);
    pthread_mutex_unlock(&queue_mutex);
}

static void frame_to_bytes(const Frame* frame, uint8_t* b0, uint8_t* b1, uint8_t* b2) {
    uint32_t shift_register = 0;
    for (int c = 0; c < CELLS_PER_FRAME; c++) {
        shift_register <<= 3;
        shift_register |= (frame->cells[c] & 0x07);
    }
    *b0 = (shift_register >> 16) & 0xFF;
    *b1 = (shift_register >> 8)  & 0xFF;
    *b2 =  shift_register        & 0xFF;
}

static bool frame_equals_bytes(const Frame* frame, const uint8_t* bytes) {
    Frame candidate;
    bytes_to_frame(bytes[0], bytes[1], bytes[2], &candidate);
    return memcmp(frame->cells, candidate.cells, CELLS_PER_FRAME) == 0;
}

// ==========================================
// PUBLIC API
// ==========================================
//buffer_length gets the size of the buffer (length of message) and returns actual amount of written bytes
uint8_t decode_queue_to_buffer(GenericQueue* queue, uint8_t* out_buf, size_t* buf_length) {
    if (!queue || !out_buf || !buf_length) return ERR_NULL_PTR;
    size_t bytes_written = 0;

    Frame current, last_frame;

    // ---- 1. Wait for START ----
    do {
        pop_from_queue(queue, &current);
    } while (!frame_equals_bytes(&current, FRAME_START));

    // ---- 2. LENGTH HEADER ----
    pop_from_queue(queue, &current);
    uint8_t b0, b1, b2;
    frame_to_bytes(&current, &b0, &b1, &b2);
    size_t expected_length = ((size_t)b0 << 16) | ((size_t)b1 << 8) | b2;

    if (expected_length > *buf_length) return ERR_BUFFER_TOO_SMALL;


    // ---- 3. FIRST DATA FRAME (no REPEAT allowed) ----
    pop_from_queue(queue, &current);
    if (frame_equals_bytes(&current, FRAME_END))    return ERR_UNEXPECTED_END;
    if (frame_equals_bytes(&current, FRAME_REPEAT)) return ERR_UNEXPECTED_REPEAT;

    if (frame_equals_bytes(&current, FRAME_ESCAPE)) {
        pop_from_queue(queue, &current);
        if (frame_equals_bytes(&current, FRAME_TYPE2_ESCAPE)) {
            b0 = FRAME_ESCAPE[0]; b1 = FRAME_ESCAPE[1]; b2 = FRAME_ESCAPE[2];
        }
        else {
            frame_to_bytes(&current, &b0, &b1, &b2);
        }
    }
    else {
        frame_to_bytes(&current, &b0, &b1, &b2);
    }

    last_frame = current;

    out_buf[bytes_written++] = b0;
    out_buf[bytes_written++] = b1;
    out_buf[bytes_written++] = b2;



    // ---- 4. DATA FRAMES ----
    while (bytes_written < expected_length - 3) {
        pop_from_queue(queue, &current);

        // END or REPEAT-as-END: stop early (corrupted/truncated stream)
        if (frame_equals_bytes(&current, FRAME_END)) return ERR_UNEXPECTED_END;

        // ---- ESCAPE sequence ----
        if (frame_equals_bytes(&current, FRAME_ESCAPE)) {
            pop_from_queue(queue, &current);
            if (frame_equals_bytes(&current, FRAME_TYPE2_ESCAPE)) {
                // ESCAPE + TYPE2_ESCAPE = literal ESCAPE bytes
                b0 = FRAME_ESCAPE[0]; b1 = FRAME_ESCAPE[1]; b2 = FRAME_ESCAPE[2];
            } else {
                // ESCAPE + anything else = literal data
                frame_to_bytes(&current, &b0, &b1, &b2);
            }
        }

        // ---- REPEAT ----
        else if (frame_equals_bytes(&current, FRAME_REPEAT)) {
            frame_to_bytes(&last_frame, &b0, &b1, &b2);
        }

        // ---- Plain data ----
        else {
            frame_to_bytes(&current, &b0, &b1, &b2);
        }

        last_frame = current;

        out_buf[(bytes_written)++] = b0;
        out_buf[(bytes_written)++] = b1;
        out_buf[(bytes_written)++] = b2;
    }


    // ---- 5. TAIL FRAME (partial, if 3, main loop will catch) ----
    if (bytes_written < expected_length) {
        pop_from_queue(queue, &current);
        if (frame_equals_bytes(&current, FRAME_END)) return ERR_UNEXPECTED_END;

        if (frame_equals_bytes(&current, FRAME_ESCAPE)) {
            pop_from_queue(queue, &current);
            if (frame_equals_bytes(&current, FRAME_TYPE2_ESCAPE)) {
                b0 = FRAME_ESCAPE[0]; b1 = FRAME_ESCAPE[1]; b2 = FRAME_ESCAPE[2];
            } else {
                frame_to_bytes(&current, &b0, &b1, &b2);
            }
        } else if (frame_equals_bytes(&current, FRAME_REPEAT)) {
            frame_to_bytes(&last_frame, &b0, &b1, &b2);
        } else {
            frame_to_bytes(&current, &b0, &b1, &b2);
        }
        last_frame = current; //in case last data was end, prepare it for actual end signal

        size_t remaining = expected_length - bytes_written;
        if (remaining >= 1) out_buf[bytes_written++] = b0;
        if (remaining >= 2) out_buf[bytes_written++] = b1;
    }

    *buf_length = bytes_written;

    // ---- 5. END ----
    pop_from_queue(queue, &current);
    if (!frame_equals_bytes(&current, FRAME_END) &&
        !(frame_equals_bytes(&current, FRAME_REPEAT) && frame_equals_bytes(&last_frame, FRAME_END)))
    {
        return ERR_MISSING_END;
    }

    return SUCCESS;
}