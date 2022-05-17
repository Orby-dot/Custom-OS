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

/*Probaby don't need this but leaving here for now

void deallocate(int level, int xPosition, freeList_t *freeListArray) {
	int buddyPosition = xPosition % 2 == 0 ? xPosition+1 : xPosition-1;
	while(level>0){
		node_t *currNode = freeListArray[level].head;
		while(currNode && currNode->startAddress<buddyPosition){
			currNode = currNode->next;
		}
		if(!currNode || currNode->startAddress!=buddyPosition){
			node_t *newNode;
			newNode->startAddress = xPosition;
			currNode->prev->next = newNode;
			newNode->next = currNode;
			currNode->prev = newNode;
			break;
		}
		level--;
		buddyPosition = (buddyPosition/2)%2==0 ? buddyPosition/2+1 : buddyPosition/2-1;
	}
}*/

void addNode(int level, int xPosition, freeList_t *freeListArray){
	node_t *currNode = freeListArray[level].head;
	
	while(currNode->next && currNode->startAddress<xPosition){
		currNode = currNode->next;
	}
	node_t *newNode;
	if(currNode->next){
		newNode->next = currNode->next;
		currNode->next->prev = newNode;
	}
	currNode->next = newNode;
	newNode->prev = currNode;
}

void removeNode(int level, int xPosition, freeList_t *freeListArray){
	node_t *currNode = freeListArray[level].head;
	
	while(currNode->startAddress!=xPosition){
		currNode = currNode->next;
	}
	currNode->prev->next = currNode->next;
	currNode->next->prev = currNode->prev;
	
	currNode->startAddress=0;
	currNode->prev=NULL;
	currNode->next=NULL;
}
