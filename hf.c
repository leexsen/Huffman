#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "buffer.h"
#include "encoder.h"
#include "decoder.h"
#include "queue.h"
#include "hf.h"

int main(int argc, char **argv)
{
	File *in, *out;
	char *srcFile, *outFile;

	if (argc < 3) {
		printf("\nusage: hf [option] input_file output_file\n"
			   "\noption:\n"
			   "  -d -decompress\n"
			   "  -c -compress\n\n");

		exit(0);
	}

	srcFile = argv[2];
	outFile = argv[3];

#ifndef DOS
	in = Fopen(srcFile, O_RDONLY, 0644);
	out = Fopen(outFile, O_WRONLY|O_CREAT|O_TRUNC, 0644);
#else
	in = Fopen(srcFile, O_RDONLY|O_BINARY);
	out = Fopen(outFile, O_WRONLY|O_CREAT|O_TRUNC|O_BINARY);
#endif

	MEM_TEST(in);
	MEM_TEST(out);

	if (strcmp(argv[1], "-d"))
		compress(in, out);
	else
		decompress(in, out);

	Fclose(in);
	Fclose(out);

	return 0;
}

void compress(File *in, File *out)
{
	Queue_Node *buffer[256] = {0};
	Queue *queueHead = encoder_countFrequency(buffer, in, out);

	Frewind(in);

	Encoder_Node *root = encoder_newEncoder(queueHead);
	Encoder_Table **table = encoder_newEncoderTable(buffer);

	uint16_t nodeCount = encoder_getEncoderNodeCount(table);
	encoder_writeHeader(root, out, in->size, nodeCount);
	encoder_writeData(table, in, out);

	encoder_freeEncoder(root);
	encoder_freeEncoderTable(table);
	queue_freeQueue(queueHead);
}

void decompress(File *in, File *out)
{
	uint32_t fileSize;
	uint16_t *bfsData;

	decoder_readHead(&fileSize, &bfsData, in);
	decoder_writeData(bfsData, fileSize, in, out);

	free(bfsData);
}
