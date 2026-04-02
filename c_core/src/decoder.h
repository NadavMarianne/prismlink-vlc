#ifndef DECODER_H
#define DECODER_H

#include <stdint.h>
#include <stddef.h>
#include "generic_queue.h"

#define CELLS_PER_FRAME 8
#define SUCCESS 0
#define ERR_NULL_PTR 1

// Define the data structure the Decoder will work with
typedef uint8_t ColorSymbol; 

typedef struct {
    ColorSymbol cells[CELLS_PER_FRAME]; 
} Frame;

// Synchronization for the JNI/Android layer
extern pthread_mutex_t queue_mutex;
extern pthread_cond_t  data_ready_cond; 
extern pthread_cond_t  can_resume_cond;

/**
 * Reconstructs 3-byte chunks from 8-cell frames.
 */
uint8_t decode_queue_to_payload(GenericQueue* queue, uint8_t* out_data, size_t expected_length);

#endif