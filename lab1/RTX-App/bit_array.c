#include "common.h"
#include "bit_array.h"

// init
void initializeBitArray(bitArray *array, U32 startAddress, U32 endAddress){
	
	array->startAddress = startAddress;
	array->endAddress = endAddress;
	array->size = startAddress-endAddress;
	
	for(int i=0;i<array->size;i++){
		array->bitStatus[i] = 0;
	}
}

// checks if the 'leftAddress' is either at the bottom of the binary tree or has two children with 0 as it's bit array value
BOOL isLeaf(bitArray* array, U32 leftAddress, U8 level){
	int heightOfTree = log(array->size);
	if(level == heightOfTree) return TRUE;
	
	int index = (leftAddress - array->startAddress);
	int leftIndex = 2*index+1;
	int rightIndex = 2*index+2;
	U8 leftBitPosition = 1<<(leftIndex%8);
	U8 rightBitPosition = 1<<(rightIndex%8);

	
	if( !(array->bitStatus[leftIndex/8] & leftBitPosition) && !(array->bitStatus[rightIndex/8] & rightBitPosition)){
		return TRUE;
	}
	
	return FALSE;
	
}


//returns index from 0 to 2^h - 1
int locateNode(bitArray* array, U32 leftAddress, U32 rightAddress, U32 targetAddress, U8 level){
	
	
	//base case where the left side of the address search is equal to the target address and it is also a leaf
	if(leftAddress == targetAddress && isLeaf(array, leftAddress, level)){
		
		int index = leftAddress - array->startAddress;
		int value = array->bitStatus[index/8];
		int bitPosition = 1<<(index/8);
		
		// not sure if this will work
		if (value & bitPosition) return RTX_ERR;
		
		return leftAddress-array->startAddress;
	}
	
	if(rightAddress<targetAddress){
		return locateNode(array, rightAddress/2, rightAddress, targetAddress, level+1);
	}
	
	return locateNode(array, leftAddress, rightAddress/2 - 1, targetAddress, level+1);	
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