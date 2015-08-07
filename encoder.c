#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encoder.h"
#include "buffer.h"
#include "queue.h"

Encoder_Node *encoder_newNode(uint16_t ch, uint8_t bit,
					  Encoder_Node *left, Encoder_Node *right)
{
	Encoder_Node *p;

	p = (Encoder_Node *)malloc(sizeof(Encoder_Node));
	MEM_TEST(p);

	p->ch = ch;
	p->bit = bit;
	p->left = left;
	p->right = right;
	p->parent = NULL;

	return p;
}

Encoder_Node *encoder_newEncoder(Queue *q)
{
	Queue_Node *qr, *ql;
	Encoder_Node *treeHead = NULL;
	Encoder_Node *er = NULL, *el = NULL;
	Queue_Node *r;

	if (q == NULL || q->head == NULL)
		return NULL;

	while (q->length - q->hasReadCount != 1 || q->length <= 1) {
    	uint32_t sum = 0;

		ql = queue_getMinNode(q);
		if (ql != NULL) {
			ql->hasRead = 1;
			q->hasReadCount++;
			el = ql->eNode;
			sum += ql->count;
		}

		qr = queue_getMinNode(q);
		if (qr != NULL) {
			qr->hasRead = 1;
			q->hasReadCount++;
			er = qr->eNode;
			sum += qr->count;
		}

		treeHead = encoder_newNode(0, 0, el, er);

		if (el != NULL) {
			el->bit = 0;
			el->parent = treeHead;
		}
		if (er != NULL) {
			er->bit = 1;
			er->parent = treeHead;
		}
		r = queue_newNode(sum, treeHead, NULL, NULL);

		queue_append(q, r);
	}

	return treeHead;
}

void encoder_freeEncoder(Encoder_Node *root)
{
	if (root == NULL)
		return;

	encoder_freeEncoder(root->left);
	encoder_freeEncoder(root->right);
	free(root);
}

Encoder_Table **encoder_newEncoderTable(Queue_Node **data)
{
	Encoder_Table **table;
	uint16_t i;
	Encoder_Table *qt, *qr;
	Encoder_Node *p;

	if (data == NULL)
		return NULL;

	table = (Encoder_Table **)calloc(sizeof(Encoder_Table *), 256);	
	MEM_TEST(table);

	for (i = 0; i < 256; i++) {
		if (data[i] != NULL) {
			qr = NULL;

			for (p = data[i]->eNode; p->parent != NULL; p = p->parent) {
				qt = (Encoder_Table *)malloc(sizeof(Encoder_Table));	
				MEM_TEST(qt);
				qt->node = p;
				qt->next = qr;
				qr = qt;
			}
			table[i] = qr;
		}
	}

	return table;
}

void encoder_freeEncoderTable(Encoder_Table **table)
{
	uint16_t i;
	Encoder_Table *t;

	if (table == NULL)
		return;

	for (i = 0; i < 256; i++) {
		while (table[i] != NULL) {
			t = table[i]->next;
			free(table[i]);
			table[i] = t;
		}
	}

	free(table);
}

static void encoder_BFS(Encoder_Node *root, uint16_t nodeCount, uint16_t *bfsData)
{
	uint16_t head = 0, tail = 0;
#ifndef DOS
	Encoder_Node *queue[nodeCount-1];
#else
	Encoder_Node **queue = (Encoder_Node **)malloc(sizeof(Encoder_Node *) * (nodeCount-1));
#endif

	if (root == NULL)
		return;

	while (head < nodeCount) {
		uint16_t ch = root->ch;

		if (root->left != NULL) {
			ch = tail+1; /* left-child node index in non-leaf node */
			queue[tail++] = root->left;		

			if (root->right != NULL)
				queue[tail++] = root->right;			

		} else
			ch |= 0x8000; /* set isLeaf flag 1(ture) */

		bfsData[head] = ch;
		root = queue[head++];	
	}

#ifdef DOS
	free(queue);
#endif
}

uint16_t encoder_getEncoderNodeCount(Encoder_Table **table)
{
	uint16_t i;
	uint16_t count = 0;

	if (table == NULL)
		return 0;

	for (i = 0; i < 256; i++) {
		if (table[i] != NULL)
			++count;
	}

	return count == 1 ? 2 : count*2 - 1;
}

void encoder_writeHeader(Encoder_Node *root, File *out, uint32_t srcFileSize, uint16_t nodeCount)
{
#ifndef DOS
	uint16_t bfsData[nodeCount];
#else
	uint16_t *bfsData = (uint16_t *)malloc(sizeof(uint16_t) * nodeCount);
#endif

	if (root == NULL || out == NULL)	
		return;

	Fwrite((const char *)&srcFileSize, sizeof(srcFileSize), 1, out);	
	Fwrite((char *)&nodeCount, sizeof(nodeCount), 1, out);	

	encoder_BFS(root, nodeCount, bfsData);

#ifndef DOS
	Fwrite((char *)bfsData, sizeof(bfsData), 1, out);	
#else
	Fwrite((char *)bfsData, sizeof(*bfsData), nodeCount, out);	
	free(bfsData);
#endif
}

void encoder_writeData(Encoder_Table **table, File *in, File *out)
{
	Encoder_Table *t;

	uint8_t ch = Fgetc(in);
	while (!Feof(in)) {

		for (t = table[ch]; t != NULL; t = t->next)
			FwriteBit(t->node->bit, out, 0);

		ch = Fgetc(in);
	}

	FwriteBit(0, out, 1);	
}
