#ifndef DECODER_H
#define DECODER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

// ==========================================
// ERROR CODES
// ==========================================

#define SUCCESS                 0x00
#define ERR_NULL_PTR            0x01
#define ERR_BUFFER_TOO_SMALL    0x02
#define ERR_UNEXPECTED_END      0x03
#define ERR_UNEXPECTED_REPEAT   0x04
#define ERR_MISSING_END         0x05

// ==========================================
// PROTOCOL CONSTANTS
// ==========================================

#define CELLS_PER_FRAME 8

extern const uint8_t FRAME_START[3];
extern const uint8_t FRAME_END[3];
extern const uint8_t FRAME_REPEAT[3];
extern const uint8_t FRAME_ESCAPE[3];
extern const uint8_t FRAME_TYPE2_ESCAPE[3];

// ==========================================
// TYPES
// ==========================================

typedef struct {
    uint8_t cells[CELLS_PER_FRAME];
} Frame;

typedef struct {
    void* data;
    int   max_capacity;
    int   item_size;
    int   front;
    int   rear;
    int   count;
} GenericQueue;

// ==========================================
// SYNC PRIMITIVES (defined in decoder.c)
// ==========================================

extern pthread_mutex_t decoder_queue_mutex;
extern pthread_cond_t  decoder_can_consume_cond;

// ==========================================
// QUEUE API
// ==========================================

void queue_pop(GenericQueue* queue, void* out);

// ==========================================
// PUBLIC API
// ==========================================

uint8_t decode_queue_to_buffer(GenericQueue* queue, uint8_t* out_buf, size_t *buf_length);

#endif // DECODER_H