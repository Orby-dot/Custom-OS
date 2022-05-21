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
<<<<<<< HEAD
U32 getHeight(bitArray*array);
U32 getXPosition(bitArray* array, U32 address, U32 level);
int locateNode(bitArray* array, U32 index);
void coalesce(bitArray *array, U32 level, U32 node);
void updateParentNodes(bitArray *array, int level, U32 node);
U32 getBitPositionMask(U32 index);
=======
U8 getHeight(bitArray*array);
U8 getXPosition(bitArray* array, U32 address, U8 level);
int locateNode(bitArray* array, U8 xPosition, U8 level);
void coalesce(bitArray *array, U8 level, U32 node);
void updateParentNodes(bitArray *array, int level, U32 node);
>>>>>>> 9e4800b3dc3b0bf2341e033bd5669d2a6fc03a9e
