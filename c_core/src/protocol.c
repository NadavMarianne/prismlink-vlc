#include "protocol.h"

/* START: SOH STX ETX - ascending sequence, extremely rare in any file */
const uint8_t FRAME_START[3]   = {0x01, 0x02, 0x03};

/* END: ETB CAN EM - extremely rare, visually distinct from START */
const uint8_t FRAME_END[3]     = {0x17, 0x18, 0x19};

/* REPEAT: ENQ ACK BEL - very rare, distinct from START and END */
const uint8_t FRAME_REPEAT[3]  = {0x05, 0x06, 0x07};

/* ESCAPE: SO SI DLE - very rare */
const uint8_t FRAME_ESCAPE[3]  = {0x0E, 0x0F, 0x10};

/* ESCAPE2: spaces - intentionally common, visually distinct from ESCAPE */
const uint8_t FRAME_TYPE2_ESCAPE[3] = {0x20, 0x20, 0x20};