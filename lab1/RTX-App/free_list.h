#include "common.h"
#include "math.h"

#define ARRAY_SIZE 0x16000
#define LEVELS 14


typedef struct node{
	U32 startAddress; // x-position
	struct node *prev;
	struct node *next;
} node_t;

typedef struct freeList {
	struct node *head;
	struct node *tail;
	
} freeList_t;

int initializeArrayOfFreeLists(freeList_t *freeListArray);

int allocate(int size, freeList_t *freeListArray);

void addNode(int level, int xPosition, freeList_t *freeListArray);

void removeNode(int level, int xPosition, freeList_t *freeListArray);