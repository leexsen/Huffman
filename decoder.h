#ifndef _DECODER_H_
#define _DECODER_H_

#include <stdint.h>
#include "general.h"

extern void decoder_readHead(uint32_t *, uint16_t *, uint16_t **, File *);
extern Encoder_Node *decoder_rebuildEncoder(uint16_t *, uint16_t, uint16_t);

#endif
