#include "common.h"
#include "free_list.h"
#include "printf.h"
#include "tester.h"

BOOL debug = FALSE;
	
U8 levels;

/**
 * freeListArray is the array of free lists
 * levelsInput is the number of levels
 * startAddress is the base address of either IRAM1 and IRAM2 
 */
int initializeArrayOfFreeLists(freeList_t *freeListArray, U8 levelsInput, U32 startAddress) {
	if (debug) printf("sa: %x \r\n", startAddress);
	levels = levelsInput;
	for (int i=0; i<levels; i++) { 
		if (i == 0) {
			// TODO: Create new node
			if (debug) printf("head called: %x \r\n", freeListArray[i].head);
			node_t *temp = (node_t *) startAddress;
			temp->next = NULL;
			temp->prev = NULL;

			(freeListArray)[i].head = (node_t *) startAddress;
			(freeListArray)[i].tail = temp;

			// printf("temp: %x \r\n", temp);
			if (debug) printf("head called: %x \r\n", (freeListArray)[i].head);
			// printf("0 Called (address of head): %x \r\n", &freeListArray[i]->head);

		} else {
			// printf("In else... %d \r\n", i);
			freeListArray[i].head = NULL;
			freeListArray[i].tail = NULL;
			// printf(	"else: %x \r\n", freeListArray[i]->head);
			// printf("else Called (address of head): %x \r\n", &freeListArray[i]->head);

		}
	}
	return 0;
}

/**
 * size: size to allocate. does NOT need to be rounded up to 2^n
 * startAddress: start address of IRAM1/IRAM2
 * returns NULL if can not allocate 
 */
U32 allocate(U32 size, freeList_t *freeListArray) {

	// find which level that block will be on
	int level = findLevel(size, levels);
	
	if (debug) printf("Allocating %u at level %d \r\n", size, level);

	// if there is a free block use it
	if(freeListArray[level].head != NULL) {
		node_t *toReturn = freeListArray[level].head;
		if (toReturn->next != NULL) toReturn->next->prev = NULL;
		freeListArray[level].head = toReturn->next;
		if (freeListArray[level].head == NULL) {
			freeListArray[level].tail = NULL;
		}
		if (debug) printf("No splitting, allocation found at %x. \r\n", (U32) toReturn);
		return (U32) toReturn;
	}
	
	// If, not go above and look for one. Once you find it, split it.
	while(freeListArray[level].head == NULL) {
		level--;
		if (level < 0) {
			return ERROR;
		}
	}

	if (debug) printf("Free node found at level %d \r\n", level);
	// remove parent node from freeList
	node_t *parent = freeListArray[level].head;
	if (parent->next != NULL) {
		parent->next->prev = NULL;
	}

	if (debug) printf(" --- parent.next %x \r\n", parent->next);
	if (freeListArray[level].head == freeListArray[level].tail) {
        freeListArray[level].head = freeListArray[level].head ->next;
        freeListArray[level].tail = freeListArray[level].head ->next;
    } 
	else {
        freeListArray[level].head = parent->next;
    }
	// freeListArray[level].head = parent->next; // don't need this

	// clear parent
	parent->next = NULL;
	parent->prev = NULL;

	// keep splitting... only keep right node of each level
	U32 parentAddress = ((U32)parent);
	
	if (debug) printf("Before while: %d < %u \r\n", level, findLevel(size, levels));

	while (level < findLevel(size, levels)) {
		// if (debug) printf("In while: %d < %u \r\n", level, findLevel(size, levels));
		// add right-node of parent to level + 1
		node_t *newRightNode = (node_t *)(parentAddress + findSize(level + 1, levels)); // this is sketch
		
		newRightNode->next = NULL; 
		newRightNode->prev = NULL;
		
		freeListArray[level + 1].head = newRightNode;
		freeListArray[level + 1].tail = newRightNode;

		level++;
	}
	
	if (debug) printf("Splitting complete, allocation found at %x. \r\n", parentAddress);

	return parentAddress;
}

void addNode(U32 level, U32 address, freeList_t *freeListArray){
	if (debug) printf(" --- FL ADD NODE lvl %d address %x \r\n", level, address);

	for (int i = 0; i < levels; i++) {
		if (debug) printf("AN Head of %d: %x addr %x\r\n", i, freeListArray[i].head, &freeListArray[i].head);
	}

	printListLevel(freeListArray[level]);

	node_t *currNode = freeListArray[level].tail;
	node_t *newNode = (node_t*) address;
	newNode->next = NULL;
	newNode->prev = NULL;

	if (debug) printf(" currnode %x \r\n", currNode);
	
	if(!currNode) {
		freeListArray[level].head = newNode;
		freeListArray[level].tail = newNode;
	}else {
		currNode->next = newNode;
		newNode->prev = currNode;
	}
	
	freeListArray[level].tail = newNode;
}

// bug not here
void removeNode(int level, U32 address, freeList_t *freeListArray){
	if (debug) printf(" --- FL REMOVE NODE lvl %d address %x \r\n", level, address);
	node_t *currNode = freeListArray[level].head;
	
	for (int i = 0; i < levels; i++) {
		if (debug) printf("RN Head of %d: %x addr %x\r\n", i, freeListArray[i].head, &freeListArray[i].head);
		
	}
	printListLevel(freeListArray[level]);

	while(currNode && (U32)currNode!=address){
		currNode = currNode->next;
	}
	
	if(currNode && currNode == freeListArray[level].head)
	{
		freeListArray[level].head = currNode->next;
	}
		
	
	if(currNode == freeListArray[level].tail)
	{
		freeListArray[level].tail = NULL;
	}
	
	if (currNode) {
		if (currNode->prev != NULL) currNode->prev->next = currNode->next;
		if (currNode->next != NULL) currNode->next->prev = currNode->prev;
		currNode->prev=NULL;
		currNode->next=NULL;
	}
}
