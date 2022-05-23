#include "common.h"
#include "free_list.h"
#include "helper.h"

#define ARRAY_SIZE	0x16000

#ifndef _BIT_ARRAY

#define _BIT_ARRAY

typedef struct bitArray{
	U32 startAddress;
	U32 endAddress;
	U32 size;
	U8 *bitStatus;
	freeList_t* freeList;
	U8 totalLevels;
} bitArray;

void initializeBitArray(bitArray *array,freeList_t * list, U8 * bitArray, U32 startAddress, U32 endAddress);
U32 allocateNode(bitArray * array, U32 sizeToAllocate);
void removeNodes(bitArray *array, U32 address);
U32 getHeight(bitArray*array);
U32 getXPosition(bitArray* array, U32 address, U32 level);
int locateNode(bitArray* array, U32 index);
void coalesce(bitArray *array, U32 level, U32 node);
void updateParentNodes(bitArray *array, U32 parentIndex);
U32 getBitPositionMask(U32 index);
U32 convertLevelToIndex(U32 level, U32 xPosition);

#endif
