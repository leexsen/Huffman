#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "buffer.h"

#ifndef DOS
#include <unistd.h>
#endif

static uint32_t FgetFileSize(const char *filename)
{
	struct stat buf;

	return (stat(filename, &buf) < 0 ? 0 : buf.st_size);
}

#ifndef DOS
File *Fopen(const char *src, int flags, mode_t mode)
#else
File *Fopen(const char *src, int flags)
#endif
{
	int fp;
	File *file = NULL;

	if (src != NULL) {
#ifndef DOS
		fp = open(src, flags, mode);
#else
		fp = open(src, flags);
#endif

		if (fp == -1)
			return NULL;

		file = (File *)malloc(sizeof(File));	
		MEM_TEST(file);

		file->fp = fp;
		file->length = 0;
		file->size = FgetFileSize(src);
		file->eof = 0;
	}

	return file;
}
	
void Fclose(File *fp)
{
	if (fp != NULL) {
		Fflush(fp);
		close(fp->fp);
		free(fp);
	}
}

#ifndef DOS
inline
#endif
int8_t Fgetc(File *fp)
{
	if (fp == NULL)
		return EOF;

	if (fp->length == 0 || (fp->index == fp->length)) {

		fp->length = read(fp->fp, fp->buf, FILE_BUF_SIZE);
		fp->index = 0;

		if (fp->length == 0) {
			fp->eof = 1;
			return EOF;
		}
	}

	return fp->buf[fp->index++];
}

#ifndef DOS
inline 
#endif
void Fputc(uint8_t ch, File *fp)
{
	if (fp != NULL) {
		fp->buf[fp->length++] = ch;

		if (fp->length == FILE_BUF_SIZE) {
			write(fp->fp, fp->buf, fp->length);
			fp->length = 0;
		}
	}
}

int Fwrite(const char *buf, int size, int count, File *fp)
{
	int i, length = count * size;

	for (i = 0; i < length; i++)
		Fputc(buf[i], fp);

	return i;
}

int Fread(char *buf, int size, int count, File *fp)
{
	int i, length = count * size;

	for (i = 0; i < length && !Feof(fp); i++)
		buf[i] = Fgetc(fp);

	return i;
}

void Fflush(File *fp)
{
	if (fp == NULL || fp->length == 0)
		return;

	write(fp->fp, fp->buf, fp->length);
	fp->length = 0;
}

uint8_t Feof(File *fp)
{
	return (fp == NULL || fp->eof);
}

void Frewind(File *fp)
{
	if (fp == NULL)
		return;

	lseek(fp->fp, 0L, SEEK_SET);
	fp->length = 0;
	fp->eof = 0;
}

void FwriteBit(uint8_t bit, File *out, uint8_t isFlush)
{
	static uint8_t buf, i;

	if (!isFlush) {
		buf <<= 1;
		buf |= bit;
		++i;

		if (i == (uint8_t)8) {
			Fputc(buf, out);
			i = 0;
			buf = 0;
		}
	} else {
		if (i != 0) {
			buf <<= 8-i;
			Fputc(buf, out);
		}
	}
}

int8_t FgetBit(uint8_t ch)
{
	static int8_t i = 7;

	if (i == -1) {
		i = 7;
		return -1;
	}

	ch >>= i--;
	return (ch & 1);
}
