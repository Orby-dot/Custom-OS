#include "common.h"

typedef struct node{
	U32 startAddress;
	U32 size;
	struct node *prev;
	struct node *next;
} node_t;

