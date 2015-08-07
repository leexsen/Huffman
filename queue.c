#include <stdio.h>
#include <stdlib.h>

#include "encoder.h"
#include "queue.h"

Queue *queue_new(void)
{
	Queue *q;
	q = (Queue *)malloc(sizeof(Queue));
	MEM_TEST(q);

	q->length = 0;
	q->hasReadCount = 0;
	q->head = NULL;
	q->tail = NULL;

	return q;
}

Queue_Node *queue_newNode(uint32_t count, Encoder_Node *p, Queue_Node *prev, Queue_Node *next)
{
	Queue_Node *q;
	q = (Queue_Node *)malloc(sizeof(Queue_Node));
	MEM_TEST(q);

	q->count = count;
	q->eNode = p;
	q->prev = prev;
	q->next = next;
	q->hasRead = 0;

	return q;
}

Queue_Node *queue_getMinNode(Queue *queue)
{
    uint32_t min = UINT32_MAX;
	Queue_Node *minNode = NULL;
	Queue_Node *q;

	if (queue == NULL)
		return NULL;

	for (q = queue->head; q != NULL; q = q->next) {
		if (q->hasRead == 0 && q->count < min) {
			minNode = q;
			min = q->count;
		}
	}

	return minNode;
}

void queue_append(Queue *q, Queue_Node *qn)
{
	if (q == NULL || qn == NULL)
		return;

	if (q->head != NULL && q->tail != NULL) {
		q->tail->next = qn;
		qn->prev = q->tail;
	} else
		q->head = qn;

	q->tail = qn;
	q->length++;
}

void queue_freeQueue(Queue *queue)
{
	if (queue == NULL)
		return;

	while (queue->head != NULL) {
		queue->tail = queue->head->next;
		free(queue->head);
		queue->head = queue->tail;
	}

	free(queue);
}
