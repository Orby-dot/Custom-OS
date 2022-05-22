#include "common.h"
#include "bit_array.h"
#include "printf.h"
#include "tester.h"

BOOL debugBA = FALSE;

// init
void initializeBitArray(bitArray *array,freeList_t * list,U8 * bitArray, U32 startAddress, U32 endAddress){
	array->bitStatus = bitArray;
	array->startAddress = startAddress;
	array->endAddress = endAddress;
	array->size = endAddress - startAddress +1;
	
	for(int i=0;i<((2*(ARRAY_SIZE)/32 -1)/8);i++){
		array->bitStatus[i] = 0;
	}
	
	array->freeList = list;
	
	initializeArrayOfFreeLists(array->freeList,log_2(endAddress-startAddress) - 4 + 1 ,startAddress);
}

// level is 1-indexed
// xPosition is 0-indexed
// the returned xPosition is 0-indexed
U32 convertLevelToIndex(U32 level, U32 xPosition){
	return (1<<(level-1))-1+xPosition;
}

U32 getBitPositionMask(U32 index){
	return 1<<(7-(index%8));
}

//returns index from 0 to 2^h - 1
int locateNode(bitArray* array, U32 index){
	if(index<=0) return 0;
	U32 bitPosition = getBitPositionMask(index);

	if( (array->bitStatus[index/8] & bitPosition) > 0){
		return index;
	}
	
	return locateNode(array, (index-1)/2);
}

// allocate memory
U32 allocateNode(bitArray * array, U32 sizeToAllocate){
	//call linked list function with sizeToAllocate, returns index within a level
	U32 address = allocate(sizeToAllocate, array->freeList); //allocate node in free list - get from free list
	if (address == ERROR ) {
		if (debugBA) printf("No space\r\n");
		return address;
	}
		
	//adding 1 to make the level 1-indexed as is consistent across bit array
	U32 level = findLevel(sizeToAllocate,levels)+1; // find level - call function from util

	U32 node = address;
	node -= array->startAddress;
	node = node/(1<<(15-level+1));
	U32 index = convertLevelToIndex(level, node);
	U32 bitPosition = getBitPositionMask(index);

	if( (array->bitStatus[index/8] & bitPosition) == 0){
		array->bitStatus[index/8] = (array->bitStatus[index/8] | bitPosition);
	}	
	
	updateParentNodes(array, (index-1)/2);
	
	return address;
	
}


void updateParentNodes(bitArray *array, U32 parentIndex){
	if(parentIndex<=0)
	{
		array->bitStatus[0] = array->bitStatus[0]| (1<<7);
		return;
	}
		
	//U32 parentIndex = (1<<level)+node-1;
	//U32 parentIndex = convertLevelToIndex(level, node);
	U32 parentPosition = getBitPositionMask(parentIndex);
	
	if( (array->bitStatus[parentIndex/8] & parentPosition) == 0){
		array->bitStatus[parentIndex/8] = array->bitStatus[parentIndex/8] | (parentPosition);
		updateParentNodes(array, (parentIndex-1)/2);
	}

}

U32 getHeight(bitArray*array){
	U32 minSize = 32;
	U32 height = log_2(array->size) - log_2(minSize)+1;
	return height;
}

U32 getXPosition(bitArray* array, U32 address, U32 level){
	return (address -array->startAddress)/(array->size >> (level-1));
}

void removeNodes(bitArray *array, U32 address){
	if (debugBA) printf(" ============== BA REMOVE NODES addr %x \r\n", address);

	U32 height = getHeight(array);
	
	//pass in array, x position on the bottom of the tree, and height
	// nodePosition is from 0 to 2^h -1
	
	//node pos in bottom level
	if (debugBA) printf("height %u \r\n", height);
	U32 xPosition = getXPosition(array, address, height);
	if (debugBA) printf("X pos %u \r\n", xPosition);
	
	//index at bottom level
	U32 index = convertLevelToIndex(height, xPosition);
	
	//find a leaf node at said address
	U32 locatedNodeIndex = 0;
	if(index>0) locatedNodeIndex = locateNode(array, (index)); // index is 0-indexed

	//check the value in the bit array
	U32 bitPosition = getBitPositionMask(locatedNodeIndex);
	
	if( (array->bitStatus[locatedNodeIndex/8] & bitPosition) > 0){
		if (debugBA) printf(" === inside if \r\n");
		//array->bitStatus[locatedNodeIndex/8] = (array->bitStatus[locatedNodeIndex/8] & ~bitPosition);
		U32 level = log_2(locatedNodeIndex+1)+1;

		U32 relativeXPosition = getXPosition(array, address, level);
		//U32 x = convertLevelToIndex(level, relativeXPosition); // flagging this - looks suspicious

		U32 buddyIndex = locatedNodeIndex;
		U32 buddyNode = relativeXPosition;
		if (relativeXPosition % 2 == 0)
		{ //buddy is on the right side
			buddyIndex++;
			buddyNode++;
		}
		else
		{ //buddy is towards the left
			buddyIndex--;
			buddyNode--;
		}

		U32 buddyBitPosition = getBitPositionMask(buddyIndex);
		if (debugBA) printf( " ----- bitPositionMask %u \r\n", buddyBitPosition);
		if (debugBA) printf(" ----- buddyIndex %u buddyNode %u \r\n", buddyIndex, buddyNode);
		if ((array->bitStatus[buddyIndex / 8] & buddyBitPosition) == 0)
		{
			coalesce(array, level, relativeXPosition);
		}
		else
		{
			array->bitStatus[index/8] = (array->bitStatus[index/8] & ~bitPosition);
			U32 address = array->startAddress + (1 << (15 - level + 1)) * (relativeXPosition);
			if (debugBA) printf(" --- address:  rfs %x \r\n", address);
			addNode(level - 1, address, array->freeList);
		}
	}
}

// coalesce

void coalesce(bitArray *array, U32 level, U32 node){
	if (debugBA) printf(" ============== BA coalesce %x lvl %u \r\n", node, level);

	if(level==1) node=0;
	U32 index = convertLevelToIndex(level, node);
	if (debugBA) printf(" -- index: %u\r\n", index);
	U32 bitPosition = getBitPositionMask(index);
	array->bitStatus[index/8] = (array->bitStatus[index/8] & ~bitPosition);
	if(level<=1) {
		return;
	}

	// this buddy calculation only makes sense for levels after the first one (1-indexed levels)
	// for levels>=2, it is always true that left children are even indexed and vice versa - this works because the number of nodes 
	// after the first level are a power of 2 which is even
	U32 buddyIndex = index;
	if (debugBA) printf(" ///////////// VAL, ADDRESS OF BUDDYINDEX: %u %x \r\n", buddyIndex, &buddyIndex);
	U32 buddyNode = node;
	if (debugBA) printf(" ///////////// VAL, ADDRESS OF BUDDYINDEX: %u %x \r\n", buddyIndex, &buddyNode);
	if(node%2==0){ //buddy is on the right side
		buddyIndex++;
		buddyNode++;
	}
	else{ //buddy is towards the left
		buddyIndex--;
		buddyNode--;
	}
	
	U32 buddyBitPosition = getBitPositionMask(buddyIndex);
	
	if( (array->bitStatus[buddyIndex/8] & buddyBitPosition) == 0 && (array->bitStatus[index/8] & bitPosition) == 0){
		//free list needs be updated to combine buddies		
		U32 address = array->startAddress+(1<<(15-level+1)) * (node);
		removeNode(level-1, address,array->freeList);
		address = array->startAddress+(1<<(15-level+1)) * (buddyNode);
		removeNode(level-1, address,array->freeList);
		if(node%2==0)
		{
			address = array->startAddress+(1<<(15-level+1)) * (node);
		}
		addNode(level-2, address, array->freeList);

		//set current node's bit array value to 0
		coalesce(array, level-1, (node)/2);
	} 

}

