#pragma once

#include <time.h>
#include <stdlib.h>

#define SWAP(A,B,TYPE) 	{ \
						TYPE tmp = A; \
						A = B; \
						B = tmp; \
						}

# define SHUFFLE(ARR,ARR_SIZE,ARR_TYPE) { \
	srand(time(0)); \
	for(int _I = 0; _I < ARR_SIZE; _I++) { \
		int _tarIdx = rand() % ARR_SIZE; \
		SWAP(ARR[_I], ARR[_tarIdx], ARR_TYPE); \
	} \
}

#define MAX(a,b) (a>b?a:b)
#define MIN(a,b) (a<b?a:b)

int mystrsplit(char ***pppList, int *pCounter, const char *pStr, const char *pSeparator);
