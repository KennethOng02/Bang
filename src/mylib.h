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
	for(int i = 0; i < ARR_SIZE; i++) { \
		int tarIdx = rand() % ARR_SIZE; \
		SWAP(ARR[i], ARR[tarIdx], ARR_TYPE); \
	} \
}


int mystrsplit(char ***pppList, int *pCounter, const char *pStr, const char *pSeparator);
