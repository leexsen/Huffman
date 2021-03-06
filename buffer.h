#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdint.h>
#include "general.h"

#define FILE_BUF_SIZE 8192 

struct file {
	int fp;
	int8_t eof;
	uint16_t index;
	uint16_t length;
    uint32_t size;
	uint8_t buf[FILE_BUF_SIZE];
};

#ifndef DOS
extern File *Fopen(const char *, int, mode_t);
#else
extern File *Fopen(const char *, int);
#endif

extern void Fclose(File *);
extern uint8_t Feof(File *);
extern int8_t Fgetc(File *);
extern void Fputc(uint8_t, File *);
extern int Fwrite(const char *, int, int, File *);
extern void FwriteBit(uint8_t, File *, uint8_t);
extern int8_t FgetBit(uint8_t);
extern int Fread(char *, int, int, File *);
extern void Fflush(File *);
extern void Frewind(File *);

#endif
