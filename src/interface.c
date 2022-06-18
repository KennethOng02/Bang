#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <ncurses.h>
#include "interface.h"
#include "avatar.h"
#include "player.h"
#include "game.h"
#include "card.h"
#include "print.h"
#include "mylib.h"

#define MSGQUE_SIZE 8
static char *msgQue[MSGQUE_SIZE] = {NULL};
static int msgQue_start = 0;

void msgQue_push(char *str) {
	if ( msgQue[msgQue_start] != NULL ) {
		free(msgQue[msgQue_start]);
	}
	if ( str != NULL ) {
		msgQue[msgQue_start] = malloc(strlen(str)+1);
		strcpy(msgQue[msgQue_start], str);
	}
	msgQue_start = (msgQue_start + 1) % MSGQUE_SIZE;
}

#define VALID_COLOR 10
#define INVALID_COLOR 11

void interface_init() {
	init_pair(VALID_COLOR, COLOR_BLACK, COLOR_GREEN);
	init_pair(INVALID_COLOR, COLOR_BLACK, COLOR_RED);
}

void moveCurDown(WINDOW *win) {
	int y, x;
	getyx(win, y, x);
	wmove(win, y + 1, 1);
	return;
}

void interface_printCenter(WINDOW *win, int start_row, char *str) {
	int center_col = getmaxx(win) / 2;
	int half_length = strlen(str) / 2;
	int adjusted_col = center_col - half_length;

	mvwprintw(win, start_row, adjusted_col, str);

	return;
}

void interface_welcome() {
	int row, col, vmid, hmid;
	getmaxyx(stdscr,row,col);
	vmid = row / 2;
	hmid = col / 2 - 15;
	attron(A_STANDOUT | A_BOLD);
	interface_printCenter(stdscr, vmid - 4, " ____________________   __________ ");
	interface_printCenter(stdscr, vmid - 3, " ___  __ )__    |__  | / /_  ____/ ");
	interface_printCenter(stdscr, vmid - 2, " __  __  |_  /| |_   |/ /_  / __   ");
	interface_printCenter(stdscr, vmid - 1, " _  /_/ /_  ___ |  /|  / / /_/ /   ");
	interface_printCenter(stdscr, vmid,     " /_____/ /_/  |_/_/ |_/  \\____/    ");
	interface_printCenter(stdscr, vmid + 2, " COPYRIGHT to Din, Brian, Kenneth  ");
	attroff(A_STANDOUT | A_BOLD);
	refresh();
	interface_printCenter(stdscr, vmid + 6, " Press any key to start...");
	getch();
	clear();
	return;
}

char *interface_askName() {
	int maxY, maxX;
	getmaxyx(stdscr, maxY, maxX);
	int y = 4;
	int x = 50;

	WINDOW *inputNameWin = newwin(y, x, maxY / 2 - y / 2, maxX / 2 - x / 2);
	box(inputNameWin, 0, 0);
	char *name = calloc(33, sizeof(char));

	while(1) {
		mvwprintw(inputNameWin, 1, 2, "Player Name: ");
		refresh();
		wrefresh(inputNameWin);

		wgetnstr(inputNameWin, name, 32);

		while(isspace((unsigned char)*name)) name++;
		
		char *end = name + strlen(name) - 1;
		while(end > name && isspace((unsigned char)*end)) end--;
		end[1] = '\0';

		if(*name != 0) {
			break;
		}
		init_pair(1, COLOR_BLACK, COLOR_YELLOW);
		wattron(inputNameWin, COLOR_PAIR(1));
		mvwprintw(inputNameWin, 2, 1, "Empty Username.");
		wattroff(inputNameWin, COLOR_PAIR(1));
	}

	delwin(inputNameWin);
	redrawwin(stdscr);
	refresh();

	return name;
}

char *interface_getCardSuit(int num) {
	char *result = calloc(4, sizeof(sizeof(char)));
	int cardNum = num % 13 + 1;
	switch (num % 4) {
		case 0:
			snprintf(result, 4, "%d%c", cardNum, 'S');
			break;
		case 1:
			snprintf(result, 4, "%d%c", cardNum, 'H');
			break;
		case 2:
			snprintf(result, 4, "%d%c", cardNum, 'D');
			break;
		case 3:
			snprintf(result, 4, "%d%c", cardNum, 'C');
			break;
		default:
			break;
	}
	return result;
}

void interface_printCards(WINDOW *win, Card **cards, bool *validCards, int selected, int l, int r) {
	for(int i = l; i < r; i++) {
		wmove(win, i-l+2, 1);
		if ( i == selected ) {
			wattron(win, COLOR_PAIR(validCards[i] ? VALID_COLOR : INVALID_COLOR));
		}
		wprintw(win, "(%d)", i+1);
		if ( cards[i] ) {
			wprintw(win, "%s[%s]", cards[i]->name, interface_getCardSuit(cards[i]->suit));
		} else {
			wprintw(win, "Unknown");
		}
		if ( i == selected ) {
			wattroff(win, COLOR_PAIR(validCards[i] ? VALID_COLOR : INVALID_COLOR));
		}
	}
	moveCurDown(win);
	wrefresh(win);
	return;
}

bool interface_yesOrNo(WINDOW *win) {
	int y, x;
	getmaxyx(win, y, x);
	mvwprintw(win, 0, x - 10, "(y/n): ");
	wrefresh(win);
	while(1) {
		char c = wgetch(win);
		switch(c) {
			case 'y':
				return true;
				break;
			case 'n':
				return false;
				break;
			default:
				wmove(win, 0, x - 3);
				clrtoeol();
				wmove(win, 0, x - 3);
				break;
		}
	}
	return false;
}

int interface_choose(Player *this, Game *game, Card **cards, bool *validCards, int cards_size, char *msg, bool notChoose) {

	int selected = 0;

	int perPage = 6;
	int start = selected - perPage/2;
	if ( start < 0 ) start = 0;
	int end = MIN(start+perPage, cards_size);

	while ( 1 ) {
		interface_refresh(this->username, game);
		refresh();

		INPUT_PRINT(msg);
		wmove(inputWin, 1, 1);
		interface_printCards(inputWin, cards, validCards, selected, start, end);

		int ch = wgetch(inputWin);
		switch ( ch ) {
		case 'q':
			Game_exit(game);
			break;
		case 'j':
			selected = (selected+1) % cards_size;
			break;
		case 'k':
			selected = (cards_size+selected-1) % cards_size;
			break;
		case '0':
			if ( notChoose ) return -1;
			break;
		case '\n':
			if ( validCards[selected] ) return selected;
			break;
		}

		if ( selected < start ) {
			start = selected;
			end = MIN(start+perPage, cards_size);
		}
		if ( selected >= end ) {
			end = selected+1;
			start = MAX(end-perPage, 0);
		}
		usleep(100);
	}
}

int *interface_chooseTake(Player *this, Game *game, Card **cards, int cards_size, int n) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);

	Card **cards_copy = malloc(cards_size * sizeof(Card *));
	for ( int i=0; i<cards_size; i++ ) {
		cards_copy[i] = cards[i];
	}
	int curSize = cards_size;

	int * rets = malloc(n * sizeof(int));
	bool validCards[cards_size];

	for ( int i=0; i<cards_size; i++ ) validCards[i] = true;
	for ( int i=n; i>=1; i-- ) {
		snprintf(buffer, bufSize, "Please choose %d cards from following list.", i);
		int idx = interface_choose(this, game, cards_copy, validCards, curSize, buffer, false);
		rets[i-1] = idx;
		for ( int j=idx; j < curSize-1; j++ ) {
			cards_copy[j] = cards_copy[j+1];
		}
		curSize--;
	}
	free(cards_copy);
	free(buffer);
	return rets;
}

int *interface_chooseDrop(Player *this, Game *game, Card **cards, int cards_size, int n) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);

	Card **cards_copy = malloc(cards_size * sizeof(Card *));
	for ( int i=0; i<cards_size; i++ ) {
		cards_copy[i] = cards[i];
	}
	int curSize = cards_size;

	int * rets = malloc(n * sizeof(int));
	bool validCards[cards_size];

	for ( int i=0; i<cards_size; i++ ) validCards[i] = true;
	for ( int i=n; i>=1; i-- ) {
		snprintf(buffer, bufSize, "Please choose %d cards from following list to drop.", i);
		int idx = interface_choose(this, game, cards_copy, validCards, curSize, buffer, false);
		rets[i-1] = idx;
		for ( int j=idx; j < curSize-1; j++ ) {
			cards_copy[j] = cards_copy[j+1];
		}
		curSize--;
	}
	free(cards_copy);
	free(buffer);
	return rets;
}

int interface_selectUse(Player *this, Game *game, Card **cards, bool *validCards, int cards_size) {
	char *msg = "Please choose a card to use";
	int ret = interface_choose(this, game, cards, validCards, cards_size, msg, true);
	if ( ret == -1 ) {
		return -1;
	}
	return ret;
}

void interface_printTargets(Player *this, Game *game, bool *validTargets, int selected, int l, int r) {
	for ( int i=0; i<game->numAvatar; i++ ) {
		Avatar *avatar = game->avatars[i];
		if ( i == selected ) {
			wattron(inputWin, COLOR_PAIR(validTargets[i] ? VALID_COLOR : INVALID_COLOR));
		}
		INPUT_PRINT("%d) %s %s%s", i+1, avatar->player->username, avatar->role == SHERIFF ? "(SHERIFF)" : "", avatar->isDead?"(DEAD)":"");
		if ( i == selected ) {
			wattroff(inputWin, COLOR_PAIR(validTargets[i] ? VALID_COLOR : INVALID_COLOR));
		}
	}
}

int interface_selectTarget(Player *this, Game *game, bool *validTargets) {
	char *msg =  "Please choose which player as target.";
	int selected = 0;
	int perPage = 6;
	int start = 0;
	int end = MIN(perPage, game->numAvatar);
	while ( 1 ) {
		interface_refresh(this->username, game);
		refresh();
		INPUT_PRINT(msg);

		interface_printTargets(this, game, validTargets, selected, start, end);

		int ch = wgetch(inputWin);
		switch ( ch ) {
		case 'q':
			Game_exit(game);
			break;
		case 'j':
			selected = (selected+1) % game->numAvatar;
			break;
		case 'k':
			selected = (game->numAvatar+selected-1) % game->numAvatar;
			break;
		case '\n':
			if ( validTargets[selected] ) return selected;
			break;
		}
		if ( selected < start ) {
			start = selected;
			end = MIN(start+perPage, game->numAvatar);
		}
		if ( selected >= end ) {
			end = selected+1;
			start = MAX(end-perPage, 0);
		}
		usleep(100);
	}
	return -1;
}

int interface_selectReact(Player *this, Game *game, Card **cards, bool *validReact, int cards_size) {
	char *msg = "Please choose a card to respond.";
	int ret = interface_choose(this, game, cards, validReact, cards_size, msg, true);
	if ( ret != -1 ) {
		return ret;
	}
	return -1;
}

bool interface_useAbility(Player *this, Game *game) {
	wprintw(inputWin, "Do you want to use your characteristic ability?");
	wrefresh(inputWin);
	return interface_yesOrNo(inputWin);
}

char *print_role(Role role) {
	if(role == SHERIFF) {
		return "SHERIFF";
	}
	if(role == DEPUTY) {
		return "DEPUTY";
	}
	if(role == OUTLAW) {
		return "OUTLAW";
	}
	if(role == RENEGADE) {
		return "RENEGADE";
	}
	if(role == ROLE_MAX) {
		return "ROLE_MAX";
	}
	return "NULL";
}

void interface_drawMessg() {
	int yMax, xMax;
	getmaxyx(stdscr, yMax, xMax);

	messgWin = newwin(10, xMax / 3, yMax - 10, xMax / 3 * 2);
	box(messgWin, 0, 0);
	//scrollok(messgWin, TRUE);
	mvwprintw(messgWin, 0, 2, "Messages");
	int idx = msgQue_start;
	for ( int row=1; row<=MSGQUE_SIZE; row++ ) {
		wmove(messgWin, row, 1);
		if ( msgQue[idx] ) {
			wprintw(messgWin, "%s", msgQue[idx]);
		}
		idx = (idx+1) % MSGQUE_SIZE;	
	}
	wrefresh(messgWin);
	//refresh();
}

void interface_drawInput(Avatar *avatar) {
	int yMax, xMax;
	getmaxyx(stdscr, yMax, xMax);

	inputWin = newwin(10, xMax / 3 * 2, yMax - 10, 0);
	box(inputWin, 0, 0);
	int xInput = getmaxx(inputWin);
	mvwprintw(inputWin, 0, 2, "%s-(%s)-(%s)-HP(%d)", avatar->player->username, print_role(avatar->role), avatar->character->name, avatar->hp);
	mvwprintw(inputWin, 0, xInput - 30, "[0]pass--[q]quit");

	wmove(inputWin, 1, 1);
	refresh();
	wrefresh(inputWin);
	return;
}

void interface_drawCardVertical(WINDOW *win, int num, int start_row) {
	int center_col = getmaxx(win) / 2;
	int half_length = 5 * num / 2;
	int adjusted_col = center_col - half_length;
	for(int i = 0; i < num; i++) mvwprintw(win, start_row, adjusted_col + i * 6,     "+---+ ");
	for(int i = 0; i < num; i++) mvwprintw(win, start_row + 1, adjusted_col + i * 6, "|   | ");
	for(int i = 0; i < num; i++) mvwprintw(win, start_row + 2, adjusted_col + i * 6, "|   | ");
	for(int i = 0; i < num; i++) mvwprintw(win, start_row + 3, adjusted_col + i * 6, "+---+ ");
	return;
}

void interface_drawCardHorizontal(WINDOW *win, int num, int start_col) {
	int center_row = getmaxy(win) / 2;
	int half_length = 3 * num / 2;
	int adjusted_row = center_row - half_length;
	/* int y, x; */
	for(int i = 0; i < num; i++) {
		mvwprintw(win, adjusted_row + i * 3, start_col,     "+-----+");
		mvwprintw(win, adjusted_row + i * 3 + 1, start_col, "|     |");
		mvwprintw(win, adjusted_row + i * 3 + 2, start_col, "+-----+");
	}
	return;
}

char *interface_getPlayerEquipment(Avatar *avatar) {
	char *info = calloc(1024, sizeof(char));
	char buffer[32] = {0};
	if(avatar->equipment->gun != NULL) {
		snprintf(buffer, 32, "GUN[%s]{%d} ", avatar->equipment->gun->name, Avatar_calcVision(avatar));
		strcat(info, buffer);
	}
	if(avatar->equipment->armour != NULL) {
		snprintf(buffer, 32, "%s ", avatar->equipment->armour->name);
		strcat(info, buffer);
	}
	if(avatar->equipment->horsePlus != NULL) {
		snprintf(buffer, 32, "H+[%s] ", avatar->equipment->horsePlus->name);
		strcat(info, buffer);
	}
	if(avatar->equipment->horseMinus != NULL) {
		snprintf(buffer, 32, "H-[%s] ", avatar->equipment->horseMinus->name);
		strcat(info, buffer);
	}
	if(avatar->equipment->jail != NULL) {
		snprintf(buffer, 32, "[JAIL] ");
		strcat(info, buffer);
	}
	if(avatar->equipment->bomb != NULL) {
		snprintf(buffer, 32, "[DYNAMITE] ");
		strcat(info, buffer);
	}
	return info;
}

void interface_printPlayerInfoHorizontal(WINDOW *win, Avatar *avatar, int y) {
	char *info = calloc(1024, sizeof(char));
	snprintf(info, 1024, "%s (%s) (%s) HP(%d)", avatar->player->username, avatar->role == SHERIFF ? "SHERIFF" : "UNKNOWN", avatar->character->name, avatar->hp);
	interface_printCenter(win, y, info);
	moveCurDown(win);
	info = interface_getPlayerEquipment(avatar);
	interface_printCenter(win, y, info);
	return;
}

void interface_printPlayerInfoVertical(WINDOW *win, Avatar *avatar, int x) {
	int yMax, xMax;
	getmaxyx(win, yMax, xMax);
	int y = yMax / 2 - avatar->cards_size / 2 * 3 - (avatar->cards_size % 2 == 0 ? 4 : 5);
	char *info = calloc(1024, sizeof(char));
	mvwprintw(win, y, x, "%s", avatar->player->username);
	mvwprintw(win, y + 1, x, "(%s) (%s)", avatar->role == SHERIFF ? "SHERIFF" : "UNKNOWN", avatar->character->name);
	mvwprintw(win, y + 2, x, "HP(%d)", avatar->hp);
	info = interface_getPlayerEquipment(avatar);
	mvwprintw(win, y + 3, x, info);

	return;
}

void interface_drawBoard(char *username, Game *game) {
	int yMax, xMax, yBoard, xBoard;
	getmaxyx(stdscr, yMax, xMax);

	boardWin = newwin(yMax - 10, xMax, 0, 0);
	getmaxyx(boardWin, yBoard, xBoard);
	idlok(boardWin, TRUE);
	scrollok(boardWin, TRUE);
	clearok(boardWin, TRUE);
	box(boardWin, 0, 0);

	interface_drawCardVertical(boardWin, 2, yBoard / 2);

	interface_drawCardVertical(boardWin, game->avatars[0]->cards_size, LINES - 16);
	char *info = interface_getPlayerEquipment(game->avatars[0]);
	interface_printCenter(boardWin, LINES - 12, info);

	interface_drawCardHorizontal(boardWin, game->avatars[1]->cards_size, 2);
	interface_printPlayerInfoVertical(boardWin, game->avatars[1], 2);

	interface_drawCardVertical(boardWin, game->avatars[2]->cards_size, 3);
	interface_printPlayerInfoHorizontal(boardWin, game->avatars[2], 1);

	interface_drawCardHorizontal(boardWin, game->avatars[3]->cards_size, COLS - 9);
	interface_printPlayerInfoVertical(boardWin, game->avatars[3], COLS - strlen((game->avatars[3]->role == SHERIFF) ? "SHERIFF" : "UNKNOWN") - strlen(game->avatars[3]->character->name) - 6);

	mvwprintw(boardWin, 0, 2, "%s's Turn", username);
	wmove(boardWin, 1, 1);
	refresh();
	wrefresh(boardWin);
	return;
}

void interface_draw(char *username, Game *game) {
	interface_drawBoard(username, game);
	interface_drawInput(game->avatars[0]);
	interface_drawMessg();
	refresh();
	return;
}

void interface_refresh(char *username, Game *game) {
	wmove(inputWin, 1, 1);
	wclrtobot(inputWin);
	interface_draw(username, game);
	return;
}
