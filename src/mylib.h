#pragma once
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

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

int mystrsplit(char ***pppList, int *pCounter, const char *pStr, const char *pSeparator) {
	if(pppList == NULL || pCounter == NULL || pStr == NULL) {
		pppList = NULL;
		errno = EINVAL;
		return -1;
	}
	
	const char * pDelim = " \n\t";
	
	int sep_len = pSeparator == NULL ? 1 : strlen(pSeparator);
	int str_len = strlen(pStr);

	if(sep_len == 0) {
		pppList = NULL;
		errno = EINVAL;
		return -1;
	}

	*pppList = malloc( (1 + str_len/sep_len ) * sizeof(char*) );

	int listIdx = 0;
	const char *pSubstr = pStr;

	char *p;
	if(pSeparator == NULL) {
		pSubstr += strspn(pSubstr, pDelim);
		p = strpbrk(pSubstr, pDelim);
	} else {
		p = strstr(pSubstr, pSeparator);
	}
	while (p != NULL) {
		(*pppList)[listIdx] = malloc(1+(p-pSubstr));
		memcpy( (*pppList)[listIdx], pSubstr, p-pSubstr);
		(*pppList)[listIdx][p-pSubstr] = '\0';

		pSubstr = p + sep_len;
		
		if(pSeparator == NULL) {
			pSubstr += strspn(pSubstr, pDelim);
			p = strpbrk(pSubstr, pDelim);
		} else {
			p = strstr(pSubstr, pSeparator);
		}
		
		listIdx++;
	}
	if(pSeparator != NULL || *pSubstr != '\0') {
		(*pppList)[listIdx] = malloc(1+str_len-(pSubstr-pStr));
		memcpy( (*pppList)[listIdx], pSubstr, 1+str_len-(pSubstr-pStr));
		listIdx++;
	}

	*pppList = realloc(*pppList, listIdx * sizeof(char*));
	*pCounter = listIdx;
	return 0;
}
