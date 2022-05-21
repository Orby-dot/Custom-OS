#include "common.h"
#include "bit_array.h"
#include "printf.h"

// init
void initializeBitArray(bitArray *array,freeList_t * list, U32 startAddress, U32 endAddress){
	
	array->startAddress = startAddress;
	array->endAddress = endAddress;
	array->size = endAddress - startAddress +1;
	
	for(int i=0;i<((2*(ARRAY_SIZE)/32 -1)/8);i++){
		array->bitStatus[i] = 0;
	}
	
	array->freeList = list;
	
	initializeArrayOfFreeLists(array->freeList,log_2(endAddress-startAddress) - 4 + 1 ,startAddress);
}

//get address from level and x position for node
/*
int getNodeAddress(bitArray* array, U8 level, U8 x){
	return ((array->size)>>level)*x;
}
*/

//returns index from 0 to 2^h - 1
int locateNode(bitArray* array, U8 xPosition, U8 level){
	
	U32 index = (1<<level)-1+xPosition;
	U8 bitPosition = 1<<(index%8);

	if( (array->bitStatus[index/8] & bitPosition) == 1){
		return index;
	}
	
	return locateNode(array, xPosition/2, level-1);
}

// allocate memory

void allocateNode(bitArray * array, U32 sizeToAllocate){
	//call linked list function with sizeToAllocate, returns index within a level
	U32 node = allocate(sizeToAllocate, array->freeList); //allocate node in free list - get from free list
	if (node == 4294967295 ) {
		printf("No space\r\n");
		return;
	}
	U8 level = findLevel(sizeToAllocate,log_2(array->size)); // find level - call function from util

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

U8 getHeight(bitArray*array){	
	U8 minSize = 32;
	U8 height = log_2(array->size) - log_2(minSize)+1;
	return height;
}

U8 getXPosition(bitArray* array, U8 address, U8 level){
	U8 height = getHeight(array);
	return (address-array->startAddress)/(array->size >> (level-1));
}

void removeNodes(bitArray *array, U32 address){
	
	U8 height = getHeight(array);
	
	//pass in array, x position on the bottom of the tree, and height
	// nodePosition is from 0 to 2^h -1
	U32 nodePosition = locateNode(array, getXPosition(array, address, height), height);
	
	//check the value in the bit array
	U8 bitPosition = 1<<(nodePosition%8);
	
	if( (array->bitStatus[nodePosition/8] & bitPosition) > 0){
		array->bitStatus[nodePosition/8] = (array->bitStatus[nodePosition/8] & ~bitPosition);
		U8 level = log_2(nodePosition+1);
		U8 x = (1<<level)-1;
		coalesce(array, level, x);
	}
	
}

// coalesce

void coalesce(bitArray *array, U8 level, U32 node){
	
	if(level<=0) return;

	U32 index = (1<<level)-1+node;
	U8 bitPosition = 1<<(index%8);

	U32 buddyIndex = index;
	U32 buddyNode = node;
	if(node%2){
		buddyIndex -=1;
		buddyNode --;
	}
	else{
		buddyIndex +=1;
		buddyNode++;
	}
	
	U8 buddyBitPosition = 1<<(buddyIndex%8);
	if( (array->bitStatus[index/8] & buddyBitPosition) == 0 && (array->bitStatus[index/8] & bitPosition) == 0){
		//free list needs be updated to combine buddies
		U32 address = array->startAddress+(1<<(15-level)) * (buddyNode-1);
		removeNode(level, address,array->freeList);
		if(!node%2)
		{
			address = array->startAddress+(1<<(15-level)) * (node-1);
		}
		addNode(level-1, address, array->freeList);
		coalesce(array, level-1, (node+1)/2);
	}


}

