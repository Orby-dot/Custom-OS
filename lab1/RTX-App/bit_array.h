#include "common.h"
#include "free_list.h"
#include "helper.h"

#define ARRAY_SIZE	0x16000

typedef struct bitArray{
	U32 startAddress;
	U32 endAddress;
	U32 size;
	U32 bitStatus[((2*(ARRAY_SIZE)/32 -1)/8)];
	freeList_t* freeList;
} bitArray;

void initializeBitArray(bitArray *array,freeList_t * list, U32 startAddress, U32 endAddress);
U32 allocateNode(bitArray * array, U32 sizeToAllocate);
void removeNodes(bitArray *array, U32 address);
U32 getHeight(bitArray*array);
U32 getXPosition(bitArray* array, U32 address, U32 level);
int locateNode(bitArray* array, U32 index);
void coalesce(bitArray *array, U32 level, U32 node);
void updateParentNodes(bitArray *array, int level, U32 node);
U32 getBitPositionMask(U32 index);
