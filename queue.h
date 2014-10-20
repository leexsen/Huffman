#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdint.h>
#include "general.h"

struct queue_node {
	uint32_t count;
	uint8_t cannotRead;	/* can't read: 1, can read: 0 */
	Encoder_Node *eNode; 
	struct queue_node *prev;
	struct queue_node *next;
};

struct queue {
	uint16_t length;
	uint16_t cannotReadCount;
	Queue_Node *head;
	Queue_Node *end;
};


extern void queue_append(Queue *, Queue_Node *);
extern Queue *queue_new(void);
extern void queue_freeQueue(Queue *);
extern Queue_Node *queue_getMinNode(Queue *);
extern Queue_Node *queue_newNode(uint32_t, Encoder_Node *, Queue_Node *, Queue_Node *);

#endif
