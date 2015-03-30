#include <stdio.h>

#include "decoder.h"
#include "encoder.h"
#include "buffer.h"

void decoder_readHead(uint32_t *size, uint16_t *nodeCount,
					  uint16_t **bfsData, File *in)
{
	if (size == NULL || nodeCount == NULL ||
		bfsData == NULL || in == NULL)
		return;

	Fread((char *)size, sizeof(*size), 1, in);
	Fread((char *)nodeCount, sizeof(*nodeCount), 1, in);

	*bfsData = (uint16_t *)malloc(*nodeCount * sizeof(**bfsData));
	MEM_TEST(*bfsData);

	Fread((char *)*bfsData, *nodeCount, sizeof(**bfsData), in);
}

Encoder_Node *decoder_rebuildEncoder(uint16_t *bfsData, uint16_t i, uint16_t n)
{
	if (i >= n)
		return NULL;

	Encoder_Node *root = encoder_newNode(bfsData[i], 0, NULL, NULL);

	// determine if this is a leaf node
	if (!(bfsData[i] & 0x8000)) {

		uint16_t index = bfsData[i]; // left-child node index

		root->left = decoder_rebuildEncoder(bfsData, index, n);
		root->right = decoder_rebuildEncoder(bfsData, index+1, n);
	}

	return root;	
}
