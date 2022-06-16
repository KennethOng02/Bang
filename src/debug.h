#pragma once
#include <stdio.h>
#include <ncurses.h>
#include "interface.h"
#include "ANSI-color-codes.h"

/* #define ERROR_PRINT(...) { \ */
/* 		fprintf(stderr, RED); \ */
/* 		fprintf(stderr, "[ERROR] File:%s Line:%d \n", __FILE__, __LINE__); \ */
/* 		fprintf(stderr, __VA_ARGS__); \ */
/* 		fprintf(stderr, COLOR_RESET); \ */
/* 		exit(-1); \ */
/* 		} */

/* #define WARNING_PRINT(...) { \ */
/* 		fprintf(stderr, YEL); \ */
/* 		fprintf(stderr, "[WARNING] "); \ */
/* 		fprintf(stderr, __VA_ARGS__); \ */
/* 		fprintf(stderr, COLOR_RESET); \ */
/* 		} */

#define ERROR_PRINT(...) { \
		printw("[ERROR] File:%s Line:%d \n", __FILE__, __LINE__); \
		exit(-1); \
		}

#define WARNING_PRINT(...) { \
		wprintw(messgWin, "[WARNING] "); \
		}

#ifdef __DEBUG__
#define DEBUG_PRINT(...) { \
		wprintw(messgWin, "[DEBUG] "); \
		}
#else
#define DEBUG_PRINT(...) 
#endif
/* #ifdef __DEBUG__ */
/* #define DEBUG_PRINT(...) { \ */
/* 		fprintf(stderr, CYN); \ */
/* 		fprintf(stderr, "[DEBUG] "); \ */
/* 		fprintf(stderr, __VA_ARGS__ ); \ */
/* 		fprintf(stderr, COLOR_RESET); \ */
/* 		} */
/* #else */
/* #define DEBUG_PRINT(...) */ 
/* #endif */
