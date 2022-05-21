#include "helper.h"

int power(int base, int exp) {
  if(exp < 0)
    return -1;

    int result = 1;
    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}

int log_2(int value) {
    int ans = 0;
    while (value >>= 1) { ++ans; }
    return ans;
}

int findLevel(int size, U8 totalLevels) {
    int pow = log_2(size - 1);
    int toReturn = -pow + totalLevels + 4 - 1;
    return toReturn > (totalLevels - 1) ? totalLevels - 1: toReturn;
}

int findSize(int level, U8 totalLevels) {
	return power(2, level*-1 + totalLevels + 4);
}

int findAddressDelta(int level, int position, U8 totalLevels) {
	return findSize(level, totalLevels)*position;
}
