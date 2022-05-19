#include "common.h"
#include "free_list.h"
#include "helper.h"
	
U8 levels;

int initializeArrayOfFreeLists(freeList_t *freeListArray, U8 levelsInput) {
	levels = levelsInput;
	for (int i=0; i<levels; i++) {
		freeList_t freeList;
		
		node_t dummyNode;
		dummyNode.prev = NULL;
		
		freeList.head = &dummyNode;
		freeList.tail = &dummyNode;
		
		if (i==0) {
			node_t startingNode;
			startingNode.next = NULL;
			startingNode.prev = &dummyNode;
			dummyNode.next = &startingNode;
		} else {
			dummyNode.next = NULL;
		}
		
		freeListArray[i] = freeList;
	}
	return 0;
}

int allocate(int size, freeList_t *freeListArray) {
	// find which level that block will be on
	int level = findLevel(size, levels);
	
	// if there is a free block use it
	if(freeListArray[level].head->next->next->next != NULL) {
		node_t *toReturn = freeListArray[level].head->next->next;
		toReturn->prev->next = toReturn->next;
		toReturn->next->prev = toReturn->prev;
		toReturn->next = NULL;
		toReturn->prev = NULL;
		
		return toReturn->startAddress;
	}
	
	// If, not go above and look for one. Once you find it, split it.
	while(freeListArray[level].head->next->next != NULL) {
		level--;
		if (level < 0) {
			return -1;
		}
	}
	
	// remove parent node from freeList
	node_t *parent = freeListArray[level].head->next->next;
	parent->prev->next = parent->next;
	parent->next->prev = parent->prev;
	parent->next = NULL;
	parent->prev = NULL;
	
	// keep splitting... only keep right node of each level
	U32 leftNodeIndex = 2*parent->startAddress;
	level++;
	
	while (level < findLevel(size, levels) - 1) {
		// add right-node of parent to level + 1
		node_t *dummyNode = freeListArray[level+1].head;
		node_t *newRightNode;
		newRightNode->startAddress = leftNodeIndex + 1;
		
		newRightNode->next = NULL;
		newRightNode->prev = dummyNode;
		dummyNode->next = newRightNode;
		freeListArray[level + 1].tail = newRightNode;
		
		leftNodeIndex = 2*leftNodeIndex;
		level++;
	}
	
	return leftNodeIndex + 1;
}

void addNode(int level, U32 address, freeList_t *freeListArray){
	node_t *currNode = freeListArray[level].tail;
	node_t *newNode = (node_t*) address;
	
	if(!currNode) {
		freeListArray[level].head = newNode;
	}else {
		currNode->next = newNode;
		newNode->prev = currNode;
	}
	
	freeListArray[level].tail = newNode;
}

void removeNode(int level, U32 address, freeList_t *freeListArray){
	node_t *currNode = freeListArray[level].head;
	
	while(currNode && (U32)currNode!=address){
		currNode = currNode->next;
	}
	
	if (currNode) {
		currNode->prev->next = currNode->next;
		currNode->next->prev = currNode->prev;

		currNode->prev=NULL;
		currNode->next=NULL;
	}
}
