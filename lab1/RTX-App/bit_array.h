#include "common.h"

#define ARRAY_SIZE	0x16000

typedef struct bitArray{
	U32 startAddress;
	U32 endAddress;
	U32 size;
	U8 bitStatus[ARRAY_SIZE];
} bitArray;

void removeNode(bitArray *array, U8 level, U32 node);
void coalesce(bitArray *array, U8 level, U32 node);
void updateParentNodes(bitArray *array, U8 level, U32 node);