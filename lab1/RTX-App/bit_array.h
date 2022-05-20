#include "common.h"
#include "free_list.h"
#include "helper.h"

#define ARRAY_SIZE	0x16000

typedef struct bitArray{
	U32 startAddress;
	U32 endAddress;
	U32 size;
	U8 bitStatus[((2*(ARRAY_SIZE)/32 -1)/8)];
	freeList_t** freeList;
} bitArray;

void initializeBitArray(bitArray *array,freeList_t ** list, U32 startAddress, U32 endAddress);
void allocateNode(bitArray * array, U32 sizeToAllocate);
void removeNodes(bitArray *array, U32 address);
U8 getHeight(bitArray*array);
U8 getXPosition(bitArray* array, U8 address, U8 level);
int locateNode(bitArray* array, U8 xPosition, U8 level);
void coalesce(bitArray *array, U8 level, U32 node);
void updateParentNodes(bitArray *array, U8 level, U32 node);
