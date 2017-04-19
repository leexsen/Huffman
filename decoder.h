#ifndef _DECODER_H_
#define _DECODER_H_

#include <stdint.h>
#include "general.h"

extern void decoder_readHead(uint32_t *, uint16_t *, uint16_t **, File *);
extern void decoder_writeData(uint16_t *, uint32_t, File *, File *);

#endif