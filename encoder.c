#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "encoder.h"
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

	return p;
}

Encoder_Node *encoder_newEncoder(Queue *q)
{
	static uint16_t num = 1;
	Queue_Node *qr, *ql;
	Encoder_Node *treeHead = NULL;
	Encoder_Node *er = NULL, *el = NULL;
	Queue_Node *r;

	if (q == NULL || q->head == NULL)
		return NULL;

	while (q->length - q->cannotReadCount != 1 || q->length <= 1) {
    	uint32_t sum = 0;

		ql = queue_getMinNode(q);
		if (ql != NULL) {
			ql->cannotRead = 1;
			q->cannotReadCount++;
			el = ql->eNode;
			sum += ql->count;
		}

		qr = queue_getMinNode(q);
		if (qr != NULL) {
			qr->cannotRead = 1;
			q->cannotReadCount++;
			er = qr->eNode;
			sum += qr->count;
		}

		treeHead = encoder_newNode(num << 8, 0, el, er);
		++num;

		if (el != NULL) {
			treeHead->left->bit = 0;
			treeHead->left->parent = treeHead;
		}
		if (er != NULL) {
			treeHead->right->bit = 1;
			treeHead->right->parent = treeHead;
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
	int i;
	Encoder_Table *qt, *qr;
	Encoder_Node *p;

	if (data == NULL)
		return NULL;

	table = (Encoder_Table **)malloc(sizeof(Encoder_Table *) * 256);	
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
	int i;
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

static void encoder_writeXian(Encoder_Node *root, File *out)
{
	if (root != NULL) {
		Fwrite((const char *)&root->ch, sizeof(root->ch), 1, out);
		encoder_writeXian(root->left, out);
		encoder_writeXian(root->right, out);
	}
}

static void encoder_writeZhong(Encoder_Node *root, File *out)
{
	if (root != NULL) {
		encoder_writeZhong(root->left, out);
		Fwrite((const char *)&root->ch, sizeof(root->ch), 1, out);
		encoder_writeZhong(root->right, out);
	}
}

uint16_t encoder_getEncoderNodeCount(Encoder_Table **table)
{
	int i;
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
	if (root == NULL || out == NULL)	
		return;

	Fwrite((const char *)&srcFileSize, sizeof(srcFileSize), 1, out);	
	Fwrite((char *)&nodeCount, sizeof(nodeCount), 1, out);	
	encoder_writeXian(root, out);
	encoder_writeZhong(root, out);
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
