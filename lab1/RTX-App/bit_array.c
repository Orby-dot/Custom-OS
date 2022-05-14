#include "common.h"
#include "bit_array.h"

// init
void initializeBitArray(bitArray *array){
	//ASK ABOUT MEMSET
	for(int i=0;i<array->size;i++){
		array->bitStatus[i] = 0;
	}
}

// allocate memory
void allocateNode(bitArray * array, U32 sizeToAllocate){
	//call linked list function with sizeToAllocate, returns level and node
	U8 level;
	U32 node;

	U32 index = (1<<level)-1+node;
	U8 bitPosition = 1<<(index%8);

	if( (array->bitStatus[index/8] & bitPosition) == 0){
		array->bitStatus[index/8] = (array->bitStatus[index/8] | bitPosition);
	}	
	
	updateParentNodes(array, level-1, (node+1)/2);
	
}

void updateParentNodes(bitArray *array, U8 level, U32 node){
	if(level<=0) return;
		
	U32 parentIndex = (1<<level)-1+node;
	U8 parentPosition = 1<<(parentIndex%8);
	
	if( (array->bitStatus[parentIndex/8] & parentPosition) == 0){
		array->bitStatus[parentIndex/8] = array->bitStatus[parentIndex/8] | (1<<parentPosition);
		updateParentNodes(array, level-1, (node+1)/2);
	}

}

// deallocate
void removeNode(bitArray *array, U8 level, U32 node){
	
	//check the value in the bit array
	U32 index = (1<<level)-1+node;
	U8 bitPosition = 1<<(index%8);
	
	if( (array->bitStatus[index/8] & bitPosition) > 0){
		array->bitStatus[index/8] = (array->bitStatus[index/8] & ~bitPosition);
		coalesce(array, level, node);
	}
	
}

// coalesce
void coalesce(bitArray *array, U8 level, U32 node){
	if(level<=0) return;

	U32 index = (1<<level)-1+node;
	U8 bitPosition = 1<<(index%8);

	U32 buddyIndex = index;
	if(node%2){
		buddyIndex +=-1;
	}
	else{
		buddyIndex +=1;
	}
	
	U8 buddyBitPosition = 1<<(buddyIndex%8);
	if( (array->bitStatus[index/8] & buddyBitPosition) == 0 && (array->bitStatus[index/8] & bitPosition) == 0){
		//free list needs be updated to combine buddies
		coalesce(array, level-1, (node+1)/2);
	}
	
	
}