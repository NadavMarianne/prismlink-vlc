#ifndef COLOR_MAP_H
#define COLOR_MAP_H

#include <stdint.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

extern const Color COLOR_MAP[8];

#endif /* COLOR_MAP_H */