#include "common.h"
#include "free_list.h"

int findLevel(int size) {
    int pow = 0;
    int i = 1;
    while(i < size) {
        i *=2;
        pow++;
    }
    return -pow + 15;
}

int initializeArrayOfFreeLists(freeList_t *freeListArray) {
	for (int i=0; i<LEVELS; i++) {
		freeList_t freeList;
		
		node_t dummyNode;
		dummyNode.prev = NULL;
		dummyNode.startAddress = -1;
		
		freeList.head = &dummyNode;
		freeList.tail = &dummyNode;
		
		if (i==0) {
			node_t startingNode;
			startingNode.next = NULL;
			startingNode.prev = &dummyNode;
			startingNode.startAddress = 0;
			dummyNode.next = &startingNode;
		} else {
			dummyNode.next = NULL;
		}
		
		freeListArray[i] = freeList;
	}
}

int allocate(int size, freeList_t *freeListArray) {
	// find which level that block will be on
	int level = findLevel(size);
	
	// if there is a free block use it
	if(freeListArray[level].head->next->next->next != NULL) {
		node_t *toReturn = freeListArray[level].head->next->next;
		toReturn->prev->next = toReturn->next;
		toReturn->next->prev = toReturn->prev;
		toReturn->next = NULL;
		toReturn->prev = NULL;
		
		return toReturn->startAddress;
	}
	// TODO: Check the number of nexts again^
	
	// If, not go above and look for one. Once you find it, split it.
	while(freeListArray[level].head->next->next != NULL) {
		level--;
		if (level < 0) {
			// exit?
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
	
	while (level < findLevel(size) - 1) {
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
	
	// return level;
	return leftNodeIndex + 1;
}
