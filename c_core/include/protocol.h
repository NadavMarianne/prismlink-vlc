#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

/* Reserved frame patterns - 3 bytes each */
extern const uint8_t FRAME_START[3];
extern const uint8_t FRAME_END[3];
extern const uint8_t FRAME_REPEAT[3];
extern const uint8_t FRAME_ESCAPE[3];
extern const uint8_t FRAME_TYPE2_ESCAPE[3];

#endif /* PROTOCOL_H */