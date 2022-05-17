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
    int pow = 0;
    int i = 1;
    while(i < size) {
        i *=2;
        pow++;
    }
    return -pow + totalLevels + 4;
}

int findSize(int level, U8 totalLevels) {
	return power(2, level*-1 + totalLevels + 4);
}

int findAddressDelta(int level, int position, U8 totalLevels) {
	return findSize(level, totalLevels)*position;
}
