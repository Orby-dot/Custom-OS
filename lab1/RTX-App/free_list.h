#include "common.h"
#include "helper.h"
#define ARRAY_SIZE 0x16000

extern U8 levels; // 1 to 11

typedef struct node {
	U32 startAddress; // x-position... not going to exist
	struct node *prev;
	struct node *next;
} node_t;

typedef struct freeList {
	struct node *head;
	struct node *tail;
} freeList_t;

int initializeArrayOfFreeLists(freeList_t *freeListArray, U8 levelsInput, U32 startAddress);

int allocate(int size, freeList_t *freeListArray);

void addNode(int level, U32 address, freeList_t *freeListArray);

void removeNode(int level, U32 address, freeList_t *freeListArray);
