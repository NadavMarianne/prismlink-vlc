#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include <stddef.h>
#include "generic_queue.h" 

// --- PrismLink Return Codes ---
#define SUCCESS       0
#define ERR_NULL_PTR  1

#define CELLS_PER_FRAME 8

typedef uint8_t ColorSymbol; 

typedef struct {
    ColorSymbol cells[CELLS_PER_FRAME];
} Frame;

// Returns PRISM_SUCCESS (0) on success, or a negative error code
uint8_t encode_payload_to_queue(const uint8_t* data, size_t data_length, GenericQueue* queue);

#endif /* ENCODER_H */