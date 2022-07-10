#include "common.h"
#include "printf.h"

#define ERROR 4294967295 // max U32

#ifndef _BUDDY_HELPER

#define _BUDDY_HELPER
// From https://stackoverflow.com/questions/25525536/write-pow-function-without-math-h-in-c
int power(int base, int exp);

// From https://stackoverflow.com/questions/3064926/how-to-write-log-base2-in-c-c
// Rounds DOWN, so log_2(3) = 1 
int log_2(int value);

// Find what level a block of a given size in B will correspond to
int findLevel(int size, U8 totalLevels);

// Find what size a levels corresponds to in B
int findSize(int level, U8 totalLevels);

// Find delta from base adress for a given node at level (y) and position (x)
int findAddressDelta(int level, int position, U8 totalLevels);

#endif
