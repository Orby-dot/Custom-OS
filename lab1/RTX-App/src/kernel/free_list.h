#include "common.h"
#include "helper.h"

#ifndef _FREE_LIST

#define _FREE_LIST

typedef struct node {
	struct node *prev;
	struct node *next;
} node_t;

typedef struct freeList {
	node_t *head;
	node_t *tail;
} freeList_t;

int initializeArrayOfFreeLists(freeList_t *freeListArray, U8 totalLevels, U32 startAddress);

U32 allocate(U32 size, freeList_t *freeListArray, U8 totalLevels);

void addNode(U32 level, U32 address, freeList_t *freeListArray, U8 totalLevels);

void removeNode(int level, U32 address, freeList_t *freeListArray, U8 totalLevels);

#endif
