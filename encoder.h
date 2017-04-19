#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <stdint.h>
#include "general.h"

struct encoder_node {
	uint8_t bit;

	/* 15th bit is leaf node flag, 1 is leaf, 0 is not
	   0-7 bits are data in leaf node
	   0-8 bits are left-child node index in non-leaf node */
	uint16_t ch;

	struct encoder_node *left;
	struct encoder_node *right;
	struct encoder_node *parent;
};

struct encoder_table {
	Encoder_Node *node;
	struct encoder_table *next;
};

extern Encoder_Node *encoder_newEncoder(Queue *);
extern void encoder_freeEncoder(Encoder_Node *);
extern Encoder_Table **encoder_newEncoderTable(Queue_Node **);
extern void encoder_freeEncoderTable(Encoder_Table **);
extern Encoder_Node *encoder_newNode(uint16_t, uint8_t, Encoder_Node *, Encoder_Node *);
extern void encoder_writeHeader(Encoder_Node *, File *, uint32_t, uint16_t);
extern void encoder_writeData(Encoder_Table **, File *, File *);
extern uint16_t encoder_getEncoderNodeCount(Encoder_Table **);
extern Queue *encoder_countFrequency(Queue_Node *[], File *, File *);

#endif
