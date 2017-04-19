#include <stdio.h>

#include "decoder.h"
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

void decoder_writeData(uint16_t *bfsData, uint32_t fileSize, File *in, File *out)
{
	uint32_t length = 0;
	uint16_t index = bfsData[0];
	uint16_t ch = Fgetc(in);

	while (!Feof(in)) {
		int8_t bit;

		while (((bit=FgetBit(ch)) != -1) && length < fileSize) {

			index = bfsData[index + bit];

			if (index & 0x8000) {
				Fputc(index, out);
				index = bfsData[0];
				++length;
			}
		}

		ch = Fgetc(in);
	}	
}
