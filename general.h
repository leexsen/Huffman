#ifndef _GENERAL_H_
#define _GENERAL_H_

#include <stdlib.h>

#define MEM_TEST(p) if (p == NULL) { \
						puts("Memory error!"); \
						exit(1);}

typedef struct queue_node Queue_Node;
typedef struct queue Queue;
typedef struct encoder_node Encoder_Node;
typedef struct encoder_table Encoder_Table;
typedef struct file File;

#endif
