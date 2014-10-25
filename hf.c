#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "buffer.h"
#include "encoder.h"
#include "decoder.h"
#include "queue.h"
#include "hf.h"

int main(int argc, char **argv)
{
	File *in, *out;
	char *srcFile, *outFile;

	if (strcmp(argv[1], "-d")) {
		srcFile = argv[1];
		outFile = argv[2];
	} else {
		srcFile = argv[2];
		outFile = argv[3];
	}

	in = Fopen(srcFile, O_RDONLY, 0644);
	MEM_TEST(in);
	out = Fopen(outFile, O_WRONLY|O_CREAT, 0644);
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
	uint8_t ch;
	Queue_Node *buf[256];
	Queue *queueHead;
	Encoder_Node *en, *root;
	Encoder_Table **table;
	Queue_Node *qn;
	uint16_t nodeCount;

	memset(buf, 0, sizeof(buf));

	queueHead = queue_new();
	ch = Fgetc(in);
	while (!Feof(in)) {
		if (buf[ch] == NULL) {
			en = encoder_newNode(ch, 0, NULL, NULL);
			qn = queue_newNode(1, en, NULL, NULL);
			queue_append(queueHead, qn);
			buf[ch] = qn;
		} else
			buf[ch]->count++;

		ch = Fgetc(in);
	}

	Frewind(in);
	root = encoder_newEncoder(queueHead);
	table = encoder_newEncoderTable(buf);
	nodeCount = encoder_getEncoderNodeCount(table);
	encoder_writeHeader(root, out, in->size, nodeCount);
	encoder_writeData(table, in, out);

	encoder_freeEncoder(root);
	encoder_freeEncoderTable(table);
	queue_freeQueue(queueHead);
}

void decompress(File *in, File *out)
{
    uint32_t fileSize, length = 0;
	uint16_t encoderNodeCount;
	uint16_t ch, *xList, *zList;
	Encoder_Node *root, *p;

	decoder_readHead(&fileSize, &encoderNodeCount, &xList, &zList, in);
	root = decoder_rebuildEncoder(xList, zList, 0, 0, encoderNodeCount);

	ch = Fgetc(in);
	p = root;
	while (!Feof(in)) {
		int8_t bit;

		while (((bit=FgetBit(ch)) != (int8_t)-1) && length < fileSize) {
			if (bit == (int8_t)0)
				p = p->left;
			else
				p = p->right;

			if (p->left == NULL && p->right == NULL) {
				Fputc(p->ch, out);
				p = root;
				++length;
			}
		}

		ch = Fgetc(in);
	}

	free(xList);
	free(zList);
	encoder_freeEncoder(root);
}
