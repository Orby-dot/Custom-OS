#include "common.h"

#define ARRAY_SIZE 0x16000

extern U8 levels; // 1 to 11

typedef struct node {
	U32 startAddress; // x-position 
	struct node *prev;
	struct node *next;
} node_t;

typedef struct freeList {
	struct node *head;
	struct node *tail;
} freeList_t;

int initializeArrayOfFreeLists(freeList_t *, U8 levelsInput);

int allocate(int size, freeList_t *freeListArray);

void addNode(int level, int xPosition, freeList_t *freeListArray);

void removeNode(int level, int xPosition, freeList_t *freeListArray);
