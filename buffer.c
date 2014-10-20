#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdint.h>

#include "buffer.h"

static uint32_t FgetFileSize(const char *filename)
{
	struct stat buf;

	return (stat(filename, &buf) < 0 ? 0 : (uint32_t)buf.st_size);
}

File *Fopen(const char *src, const char *flag)
{
	FILE *fp;
	File *file = NULL;

	if (src != NULL && flag != NULL) {
		fp = fopen(src, flag);
		MEM_TEST(fp);
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
		fclose(fp->fp);
		free(fp);
	}
}

int8_t Fgetc(File *fp)
{
	if (fp == NULL)
		return EOF;

	if (fp->length == 0 || (fp->index == fp->length)) {
		fp->length = fread(fp->buf, 1, FILE_BUF_SIZE, fp->fp);
		fp->index = 0;
		if (fp->length == 0) {
			fp->eof = 1;
			return EOF;
		}
	}

	return fp->buf[fp->index++];
}

void Fgets(char *buf, int bufsize, File *fp)
{
	int i;

	if (buf == NULL || fp == NULL || bufsize == 0)
		return;

	for (i = 0; i < bufsize-1; i++)
		buf[i] = Fgetc(fp);

	buf[bufsize-1] = '\0';
}

void Fputc(uint8_t ch, File *fp)
{
	if (fp != NULL) {
		fp->buf[fp->length++] = ch;

		if (fp->length == FILE_BUF_SIZE) {
			fwrite(fp->buf, fp->length, 1, fp->fp);
			fp->length = 0;
		}
	}
}

void Fputs(const char *str, File *fp)
{
	int i;
	if (str == NULL || fp == NULL)
		return;

	for (i = 0; str[i] != '\0'; i++)
		Fputc(str[i], fp);
}

int Fwrite(const char *buf, int size, int count, File *fp)
{
	int i, length = count * size;

	for (i = 0; i < length && !Feof(fp); i++)
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
	if (fp == NULL)
		return;

	fwrite(fp->buf, fp->length, 1, fp->fp);
	fp->length = 0;
}

uint8_t Feof(File *fp)
{
	return (fp == NULL || (fp->eof == 1) ? 1 : 0);
}

void Frewind(File *fp)
{
	if (fp == NULL)
		return;

	rewind(fp->fp);
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
	uint8_t bit = 0x80;
	static uint8_t i = 0;

	if (i == 8) {
		i = 0;
		return -1;
	}

	bit >>= i++;
	return (((bit & ch) == bit) ? 1 : 0);
}
