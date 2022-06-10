#pragma once
#include <stdio.h>

#define ERROR_PRINT(...) {\
		fprintf(stderr, "[ERROR] File:%s Line:%d \n", __FILE__, __LINE__); \
		fprintf(stderr, __VA_ARGS__); \
		exit(-1);}

#define WARNING_PRINT(...) \
		fprintf(stderr, "[WARNING] "); \
		fprintf(stderr, __VA_ARGS__)

#ifdef __DEBUG__
#define DEBUG_PRINT(...) \
		fprintf(stderr, "[DEBUG] "); \
		fprintf(stderr, __VA_ARGS__ )
#else
#define DEBUG_PRINT(...) 
#endif
