#include "common.h"
#include "helper.h"

#ifndef _FREE_LIST

#define _FREE_LIST

extern U8 levels; // 1 to 11

typedef struct node {
	struct node *prev;
	struct node *next;
} node_t;

typedef struct freeList {
	node_t *head;
	node_t *tail;
} freeList_t;

int initializeArrayOfFreeLists(freeList_t *freeListArray, U8 levelsInput, U32 startAddress);

U32 allocate(U32 size, freeList_t *freeListArray);

void addNode(int level, U32 address, freeList_t *freeListArray);

void removeNode(int level, U32 address, freeList_t *freeListArray);

#endif
