#include <stdio.h>

#include "decoder.h"
#include "encoder.h"
#include "buffer.h"

void decoder_readHead(uint32_t *size, uint16_t *nodeCount,
					  uint16_t **xList, uint16_t **zList, File *in)
{
	if (size == NULL || nodeCount == NULL ||
		xList == NULL || zList == NULL || in == NULL)
		return;

	Fread((char *)size, sizeof(*size), 1, in);
	Fread((char *)nodeCount, sizeof(*nodeCount), 1, in);

	*xList = (uint16_t *)malloc(*nodeCount * sizeof(uint16_t));
	MEM_TEST(*xList);
	*zList = (uint16_t *)malloc(*nodeCount * sizeof(uint16_t));
	MEM_TEST(*zList);

	Fread((char *)*xList, (int)*nodeCount, 2, in);
	Fread((char *)*zList, (int)*nodeCount, 2, in);
}

Encoder_Node *decoder_rebuildEncoder(uint16_t *xList, uint16_t *zList,
									 uint16_t xS, uint16_t zS, uint16_t n)
{
	int i;
	Encoder_Node *root;
		
	if (n <= 0)
		return NULL;
	
	root = encoder_newNode(xList[xS], 0, NULL, NULL);
	for (i = 0; i < n && (xList[xS] != zList[zS+i]); i++);
	
	root->left = decoder_rebuildEncoder(xList, zList, xS+1, zS, i);
	root->right = decoder_rebuildEncoder(xList, zList, xS+i+1, zS+i+1, n-i-1);
	
	return root;	
}
