#pragma once
#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include "interface.h"
#include "ANSI-color-codes.h"

#define ERROR_PRINT(...) { \
		fprintf(stderr, RED); \
		fprintf(stderr, "[ERROR] File:%s Line:%d \n", __FILE__, __LINE__); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, COLOR_RESET); \
		exit(-1); \
		}

/* #define WARNING_PRINT(...) { \ */
/* 		fprintf(stderr, YEL); \ */
/* 		fprintf(stderr, "[WARNING] "); \ */
/* 		fprintf(stderr, __VA_ARGS__); \ */
/* 		fprintf(stderr, COLOR_RESET); \ */
/* 		} */

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

#define WARNING_PRINT(...) { \
	init_pair(1, COLOR_BLACK, COLOR_YELLOW); \
	wattron(messgWin, COLOR_PAIR(1)); \
	wprintw(messgWin, "[WARNING]"); \
	wprintw(messgWin, __VA_ARGS__); \
	wattroff(messgWin, COLOR_PAIR(1)); \
	moveCurDown(messgWin); \
	wrefresh(messgWin); \
	/*sleep(1);*/ \
	usleep(500); \
}

#define MESSAGE_PRINT(...) { \
	/*wprintw(messgWin, __VA_ARGS__); \
	moveCurDown(messgWin); \
	wrefresh(messgWin); \
	usleep(500); */ \
	int bufSize = 128; \
	char *buffer = malloc(bufSize); \
	snprintf(buffer, bufSize, __VA_ARGS__); \
	msgQue_push(buffer); \
	usleep(3000); \
}

#define INPUT_PRINT(...) { \
	wprintw(inputWin, __VA_ARGS__); \
	moveCurDown(inputWin); \
	wrefresh(inputWin); \
}

#ifdef __DEBUG__
#define DEBUG_PRINT(...) { \
	init_pair(3, COLOR_BLACK, COLOR_MAGENTA); \
	wattron(messgWin, COLOR_PAIR(3)); \
	wprintw(messgWin, "[DEBUG]"); \
	wprintw(messgWin, __VA_ARGS__); \
	wattroff(messgWin, COLOR_PAIR(3)); \
	moveCurDown(messgWin); \
	wrefresh(messgWin); \
	sleep(1); \
}
#else
#define DEBUG_PRINT(...) 
#endif
