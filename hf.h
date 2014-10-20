#ifndef _HF_H_
#define _HF_H_

#include <stdint.h>
#include "general.h"
#include "buffer.h"

extern void compress(File *, File *);
extern void decompress(File *, File *);

#endif
