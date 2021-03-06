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
	curs_set(0);
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
	int half_col = getmaxx(win) / 2;
	int half_length = strlen(str) / 2;
	int adjusted_col = half_col - half_length;

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
	char *name = calloc(16, sizeof(char));

	while(1) {
		mvwprintw(inputNameWin, 1, 2, "Player Name: ");
		refresh();
		wrefresh(inputNameWin);

		wgetnstr(inputNameWin, name, 15);

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
	switch (num / 13) {
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

bool interface_yesOrNo(WINDOW *win, Game *game, Player *this, char *msg) {
	int y, x;
	getmaxyx(win, y, x);
	while(1) {
		interface_drawInput(this->avatar, false, true, false, false);
		wmove(win, 1, 1);
		wprintw(win, "%s", msg);
		mvwprintw(win, y - 1, x - 10, "(y/n): ");
		wrefresh(win);
		char c = wgetch(win);
		switch(c) {
			case 'Q':
				Game_exit(game);
				break;
			case 'i':
				interface_drawInfo(game);
				break;
			case 'y':
				return true;
				break;
			case 'n':
				return false;
				break;
			default:
				wmove(win, y - 1, x - 3);
				clrtoeol();
				wmove(win, y - 1, x - 3);
				break;
		}
		usleep(100);
	}
	return false;
}

int interface_choose(Player *this, Game *game, Card **cards, bool *validCards, int cards_size, char *msg, bool notChoose, bool canBack, bool canUseAbility) {

	int selected = 0;

	int perPage = 6;
	int start = selected - perPage/2;
	if ( start < 0 ) start = 0;
	int end = MIN(start+perPage, cards_size);

	flushinp();
	while ( 1 ) {
		interface_drawInput(this->avatar, notChoose, true, canBack, canUseAbility);
		INPUT_PRINT(msg);
		wmove(inputWin, 1, 1);
		interface_printCards(inputWin, cards, validCards, selected, start, end);

		int ch = wgetch(inputWin);
		switch ( ch ) {
		case 'Q':
			Game_exit(game);
			break;
		case 'j':
		case KEY_DOWN:
			if ( cards_size > 0) selected = (selected+1) % cards_size;
			break;
		case 'k':
		case KEY_UP:
			if ( cards_size > 0) selected = (cards_size+selected-1) % cards_size;
			break;
		case '0':
			if ( notChoose ) {
				interface_drawInput(this->avatar, false, false, false, false);
				wrefresh(inputWin);
				return -1;
			}
			break;
		case 'i':
			interface_drawInfo(game);
			break;
		case '\n':
			if ( cards_size > 0 && validCards[selected] ) {
				interface_drawInput(this->avatar, false, false, false, false);
				wrefresh(inputWin);
				return selected;
			}
			break;
		case 'u':
			if ( canBack ) {
				interface_drawInput(this->avatar, false, false, false, false);
				wrefresh(inputWin);
				return -1;
			}
			break;
		case 'a':
			if ( canUseAbility ) {
				return -2;
			}
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

int *interface_chooseTake(Player *this, Game *game, Card **cards, int cards_size, int n, bool undo) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);

	Card **cards_copy = malloc(cards_size * sizeof(Card *));
	int *indexes_copy = malloc(cards_size * sizeof(int));
	for ( int i=0; i<cards_size; i++ ) {
		cards_copy[i] = cards[i];
		indexes_copy[i] = i;
	}
	int curSize = cards_size;

	int * rets = malloc(n * sizeof(int));
	bool validCards[cards_size];

	for ( int i=0; i<cards_size; i++ ) validCards[i] = true;
	for ( int i=n; i>=1; i-- ) {
		snprintf(buffer, bufSize, "Please choose %d cards from following list.", i);
		int idx = interface_choose(this, game, cards_copy, validCards, curSize, buffer, false, undo, false);
		if ( idx == -1 ) { // i != n
			i++;
			curSize++;
			Card *tmp = cards_copy[curSize-1];
			int tmpIdx = indexes_copy[curSize-1];
			for ( int j=curSize-2; j>=tmpIdx; j-- ) {
				cards_copy[j+1] = cards_copy[j];
				indexes_copy[j+1] = indexes_copy[j];
			}
			cards_copy[tmpIdx] = cards[rets[i-1]];
			indexes_copy[tmpIdx] = rets[i-1];
			i++;
			continue;
		}
		int tmpIdx = idx;
		rets[i-1] = indexes_copy[idx];
		Card *tmp = cards[rets[i-1]];
		for ( int j=idx; j < curSize-1; j++ ) {
			cards_copy[j] = cards_copy[j+1];
			indexes_copy[j] = indexes_copy[j+1];
		}
		cards_copy[curSize-1] = tmp;
		indexes_copy[curSize-1] = tmpIdx;
		curSize--;
	}
	free(indexes_copy);
	free(cards_copy);
	free(buffer);
	return rets;
}

int *interface_chooseDrop(Player *this, Game *game, Card **cards, int cards_size, int n, bool notChoose) {
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

	for ( int i=n; i>=1; i-- ) { // Don't change this!
		snprintf(buffer, bufSize, "Please choose %d cards from following list to drop.", i);
		bool canBack = false;
		if ( notChoose || i!=n ) {
			canBack = true;
		}
		int idx = interface_choose(this, game, cards_copy, validCards, curSize, buffer, false, canBack, false);
		if ( idx == -1 ) {
			if ( i != n ) {
				// Undo
				i++;
				curSize++;
				Card *tmp = cards_copy[curSize-1];
				for ( int j=curSize-2; j >= rets[i-1]; j-- ) {
					cards_copy[j+1] = cards_copy[j];
				}
				cards_copy[rets[i-1]] = tmp;
				i++;
				continue;
			} else {
				// not choose
				free(cards_copy);
				free(buffer);
				return NULL;
			}
		}
		Card *tmp = cards_copy[idx];
		rets[i-1] = idx;
		for ( int j=idx; j < curSize-1; j++ ) {
			cards_copy[j] = cards_copy[j+1];
		}
		cards_copy[curSize-1] = tmp;
		curSize--;
	}
	free(cards_copy);
	free(buffer);
	return rets;
}

int interface_selectUse(Player *this, Game *game, Card **cards, bool *validCards, int cards_size, bool canUseAbility) {
	char *msg = "Please choose a card to use";
	int ret = interface_choose(this, game, cards, validCards, cards_size, msg, true, false, canUseAbility);
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
	flushinp();
	while ( 1 ) {
		interface_drawInput(this->avatar, false, true, true, false);
		INPUT_PRINT(msg);

		interface_printTargets(this, game, validTargets, selected, start, end);
		wrefresh(inputWin);

		int ch = wgetch(inputWin);
		switch ( ch ) {
		case 'Q':
			Game_exit(game);
			break;
		case 'j':
		case KEY_DOWN:
			selected = (selected+1) % game->numAvatar;
			break;
		case 'k':
		case KEY_UP:
			selected = (game->numAvatar+selected-1) % game->numAvatar;
			break;
		case 'i':
			interface_drawInfo(game);
			break;
		case '\n':
			//wclear(inputWin);
			interface_drawInput(this->avatar, false, false, false, false);
			wrefresh(inputWin);
			if ( validTargets[selected] ) return selected;
			break;
		case 'u':
			interface_drawInput(this->avatar, false, false, false, false);
			wrefresh(inputWin);
			return -1;
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

int interface_selectReact(Player *this, Game *game, Card **cards, bool *validReact, int cards_size, bool canUseAbility) {
	char *msg = "Please choose a card to respond.";
	int ret = interface_choose(this, game, cards, validReact, cards_size, msg, true, false, canUseAbility);
	return ret;
}

bool interface_useAbility(Player *this, Game *game) {
	int bufSize = 128;
	char *buffer = malloc(bufSize);
	snprintf(buffer, bufSize, "Do you want to use your characteristic ability(%s)?", this->avatar->character->name);
	bool ret = interface_yesOrNo(inputWin, game, this, buffer);
	free(buffer);
	return ret;
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

	messgWin = newwin(10, xMax / 2, yMax - 10, xMax / 2);
	box(messgWin, 0, 0);
	mvwprintw(messgWin, 0, 2, "Messages");

	int wyMax, wxMax;
	getmaxyx(messgWin, wyMax, wxMax);
	int perLine = wxMax-2;
	int idx = (MSGQUE_SIZE+msgQue_start-1) % MSGQUE_SIZE;
	char *trav = msgQue[idx];
	int lineCount = 0;
	while ( 1 ) {
		int line = 1;
		if ( trav ) line = (strlen(trav)-1) / perLine + 1; 
		if ( lineCount + line <= MSGQUE_SIZE ) {
			lineCount += line;
			idx = (MSGQUE_SIZE+idx-1) % MSGQUE_SIZE;
			trav = msgQue[idx];
		} else {
			break;
		}
	}

	if ( lineCount < MSGQUE_SIZE ) {
		trav += perLine * ( lineCount + (strlen(trav)-1)/perLine+1 - MSGQUE_SIZE);
	} else {
		idx = (idx+1) % MSGQUE_SIZE;	
		trav = msgQue[idx];
	}
	lineCount = 0;
	do {
		wmove(messgWin, lineCount+1, 1);
		if ( trav ) {
			wprintw(messgWin, "%.*s", perLine, trav);
			if ( strlen(trav) <= perLine ) {
				idx = (idx+1) % MSGQUE_SIZE;
				trav = msgQue[idx];
			} else {
				trav += perLine;
			}
		} else {
			idx = (idx+1) % MSGQUE_SIZE;	
			trav = msgQue[idx];
		}
		lineCount++;
	} while ( idx != msgQue_start );
	wrefresh(messgWin);
}

void interface_drawInput(Avatar *avatar, bool canPass, bool canQuit, bool canBack, bool canUseAbility) {
	int yMax, xMax;
	getmaxyx(stdscr, yMax, xMax);

	inputWin = newwin(10, xMax / 2, yMax - 10, 0);
	keypad(inputWin, true);
	box(inputWin, 0, 0);
	int yInput, xInput;
	getmaxyx(inputWin, yInput, xInput);
	char *buffer = calloc(64, sizeof(char));
	snprintf(buffer, 64, "%s-(%s)-(%s)-HP(%d/%d)", avatar->player->username, print_role(avatar->role), avatar->character->name, avatar->hp, avatar->hp_max);
	mvwprintw(inputWin, 0, 1, buffer);
	int offset = 1;
	if ( canPass ) {
		mvwprintw(inputWin, yInput - 1, offset, "[0]pass");
	}
	if ( canQuit ) {
		mvwprintw(inputWin, yInput - 1, offset + 8, "[Q]quit");
	}
	if (canQuit) {
		mvwprintw(inputWin, yInput - 1, offset + 16, "[i]info");
	}
	if ( canBack ) {
		mvwprintw(inputWin, yInput - 1, offset + 24, "[u]undo");
	}
	if ( canUseAbility ) {
		mvwprintw(inputWin, yInput - 1, offset + 24, "[a]ability");
	}

	wmove(inputWin, 1, 1);
	refresh();
	wrefresh(inputWin);
	return;
}

void interface_drawCardVertical(WINDOW *win, int num, int start_row, double xratio) {
	int center_col = getmaxx(win) * xratio;
	int half_length = 5 * num / 2;
	int adjusted_col = center_col - half_length;
	for(int i = 0; i < num; i++) mvwprintw(win, start_row, adjusted_col + i * 6,     "+---+ ");
	for(int i = 0; i < num; i++) mvwprintw(win, start_row + 1, adjusted_col + i * 6, "|   | ");
	for(int i = 0; i < num; i++) mvwprintw(win, start_row + 2, adjusted_col + i * 6, "|   | ");
	for(int i = 0; i < num; i++) mvwprintw(win, start_row + 3, adjusted_col + i * 6, "+---+ ");
	return;
}

void interface_drawCardHorizontal(WINDOW *win, int num, int start_col, double yratio) {
	int center_row = getmaxy(win) * yratio;
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
	char *end = info + strlen(info) - 1;
	while(end > info && isspace((unsigned char)*end)) end--;
	end[1] = '\0';
	return info;
}

void interface_printxCenter(WINDOW *win, int start_row, int mid_col, char *str) {
	int half_len = strlen(str) / 2;
	int adj_col = mid_col - half_len;
	mvwprintw(win, start_row, adj_col, str);
}

void interface_printPlayerInfoHorizontal(WINDOW *win, Avatar *avatar, int y, double xratio) {
	char *info = calloc(1024, sizeof(char));
	int col = getmaxx(win) * xratio;

	snprintf(info, 1024, "%s (%s)", avatar->player->username, avatar->role == SHERIFF || avatar->isDead ? print_role(avatar->role) : "Unknown");
	interface_printxCenter(win, y, col, info);

	snprintf(info, 1024, "%s",avatar->character->name);
	interface_printxCenter(win, y+1, col, info);

	snprintf(info, 1024, "HP(%d/%d)", avatar->hp, avatar->hp_max);
	interface_printxCenter(win, y+2, col, info);

	info = interface_getPlayerEquipment(avatar);
	interface_printxCenter(win, y+3, col, info);
	return;
}

void interface_printPlayerInfoVertical(WINDOW *win, Avatar *avatar, int x, double yratio) {
	int yMax, xMax;
	getmaxyx(win, yMax, xMax);
	//int y = yMax / 2 - avatar->cards_size / 2 * 3 - (avatar->cards_size % 2 == 0 ? 4 : 5);
	int y = yMax * yratio;
	int bufSize = 128;
	char *buffer = calloc(bufSize, sizeof(char));

	int lineCount = 2;
	for ( Card **iter = (Card **)avatar->equipment; iter < (Card **)(avatar->equipment+1); iter++ ) {
		if ( *iter ) lineCount++;
	}

	int row = y - lineCount / 2;
	int left;
	snprintf(buffer, bufSize, "%s (%s)", avatar->player->username, avatar->role == SHERIFF || avatar->isDead ? print_role(avatar->role) : "Unknown");
	left = x - strlen(buffer) / 2;
	mvwprintw(win, row++, left, "%s", buffer);

	snprintf(buffer, bufSize, "%s", avatar->character->name);
	left = x - strlen(buffer) / 2;
	mvwprintw(win, row++, left, "%s", buffer);

	snprintf(buffer, bufSize, "HP(%d/%d)", avatar->hp, avatar->hp_max);
	left = x - strlen(buffer) / 2;
	mvwprintw(win, row++, left, "%s", buffer);

	if(avatar->equipment->gun != NULL) {
		snprintf(buffer, bufSize, "GUN[%s]{%d} ", avatar->equipment->gun->name, Avatar_calcVision(avatar));
		left = x - strlen(buffer) / 2;
		mvwprintw(win, row++, left, "%s", buffer);
	}
	if(avatar->equipment->armour != NULL) {
		snprintf(buffer, bufSize, "%s", avatar->equipment->armour->name);
		left = x - strlen(buffer) / 2;
		mvwprintw(win, row++, left, "%s", buffer);
	}
	if(avatar->equipment->horsePlus != NULL) {
		snprintf(buffer, bufSize, "H+[%s] ", avatar->equipment->horsePlus->name);
		left = x - strlen(buffer) / 2;
		mvwprintw(win, row++, left, "%s", buffer);
	}
	if(avatar->equipment->horseMinus != NULL) {
		snprintf(buffer, bufSize, "H-[%s] ", avatar->equipment->horseMinus->name);
		left = x - strlen(buffer) / 2;
		mvwprintw(win, row++, left, "%s", buffer);
	}
	if(avatar->equipment->jail != NULL) {
		snprintf(buffer, bufSize, "[JAIL] ");
		left = x - strlen(buffer) / 2;
		mvwprintw(win, row++, left, "%s", buffer);
	}
	if(avatar->equipment->bomb != NULL) {
		snprintf(buffer, bufSize, "[DYNAMITE] ");
		left = x - strlen(buffer) / 2;
		mvwprintw(win, row++, left, "%s", buffer);
	}
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

	interface_drawCardVertical(boardWin, 2, yBoard / 2, 1.0/2.0);

	if ( game->numAvatar < 7 ) {
		interface_drawCardVertical(boardWin, game->avatars[0]->cards_size, LINES - 16, 1.0/2.0);
		char *info = interface_getPlayerEquipment(game->avatars[0]);
		int mid_col = getmaxx(boardWin) / 2;
		interface_printxCenter(boardWin, LINES - 12, mid_col, info);
	}

	if ( game->numAvatar == 4 ) {
		interface_drawCardHorizontal(boardWin, game->avatars[1]->cards_size, 2, 3.0/5.0);
		interface_printPlayerInfoVertical(boardWin, game->avatars[1], 22, 3.0/5.0);

		interface_drawCardVertical(boardWin, game->avatars[2]->cards_size, 1, 1.0/2.0);
		interface_printPlayerInfoHorizontal(boardWin, game->avatars[2], 5, 1.0/2.0);

		interface_drawCardHorizontal(boardWin, game->avatars[3]->cards_size, COLS - 9, 3.0/5.0);
		interface_printPlayerInfoVertical(boardWin, game->avatars[3], COLS - 21, 3.0/5.0);

	} else if ( game->numAvatar == 5 ) {

		interface_drawCardHorizontal(boardWin, game->avatars[1]->cards_size, 2, 3.0/5.0);
		interface_printPlayerInfoVertical(boardWin, game->avatars[1], 22, 3.0/5.0);

		interface_drawCardVertical(boardWin, game->avatars[2]->cards_size, 1, 1.0/3.0);
		interface_printPlayerInfoHorizontal(boardWin, game->avatars[2], 5, 1.0/3.0);

		interface_drawCardVertical(boardWin, game->avatars[3]->cards_size, 1, 2.0/3.0);
		interface_printPlayerInfoHorizontal(boardWin, game->avatars[3], 5, 2.0/3.0);

		interface_drawCardHorizontal(boardWin, game->avatars[4]->cards_size, COLS - 9, 3.0/5.0);
		interface_printPlayerInfoVertical(boardWin, game->avatars[4], COLS - 21, 3.0/5.0);

	} else if ( game->numAvatar == 6 ) {

		interface_drawCardHorizontal(boardWin, game->avatars[1]->cards_size, 2, 3.0/5.0);
		interface_printPlayerInfoVertical(boardWin, game->avatars[1], 22, 3.0/5.0);

		interface_drawCardVertical(boardWin, game->avatars[2]->cards_size, 1, 1.0/5.0);
		interface_printPlayerInfoHorizontal(boardWin, game->avatars[2], 5, 1.0/5.0);

		interface_drawCardVertical(boardWin, game->avatars[3]->cards_size, 1, 2.0/4.0);
		interface_printPlayerInfoHorizontal(boardWin, game->avatars[3], 5, 2.0/4.0);

		interface_drawCardVertical(boardWin, game->avatars[4]->cards_size, 1, 4.0/5.0);
		interface_printPlayerInfoHorizontal(boardWin, game->avatars[4], 5, 4.0/5.0);

		interface_drawCardHorizontal(boardWin, game->avatars[5]->cards_size, COLS - 9, 3.0/5.0);
		interface_printPlayerInfoVertical(boardWin, game->avatars[5], COLS - 21, 3.0/5.0);

	} else if ( game->numAvatar == 7 ) {

		interface_drawCardVertical(boardWin, game->avatars[0]->cards_size, LINES - 16, 1.0/3.0);
		char *info = interface_getPlayerEquipment(game->avatars[0]);
		int mid_col = getmaxx(boardWin) / 3;
		interface_printxCenter(boardWin, LINES - 12, mid_col, info);

		interface_drawCardHorizontal(boardWin, game->avatars[1]->cards_size, 2, 3.0/5.0);
		interface_printPlayerInfoVertical(boardWin, game->avatars[1], 22, 3.0/5.0);

		interface_drawCardVertical(boardWin, game->avatars[2]->cards_size, 1, 1.0/5.0);
		interface_printPlayerInfoHorizontal(boardWin, game->avatars[2], 5, 1.0/5.0);

		interface_drawCardVertical(boardWin, game->avatars[3]->cards_size, 1, 2.0/4.0);
		interface_printPlayerInfoHorizontal(boardWin, game->avatars[3], 5, 2.0/4.0);

		interface_drawCardVertical(boardWin, game->avatars[4]->cards_size, 1, 4.0/5.0);
		interface_printPlayerInfoHorizontal(boardWin, game->avatars[4], 5, 4.0/5.0);

		interface_drawCardHorizontal(boardWin, game->avatars[5]->cards_size, COLS - 9, 3.0/5.0);
		interface_printPlayerInfoVertical(boardWin, game->avatars[5], COLS - 21, 3.0/5.0);

		interface_drawCardVertical(boardWin, game->avatars[6]->cards_size, LINES - 16, 2.0/3.0);
		info = interface_getPlayerEquipment(game->avatars[6]);
		mid_col = getmaxx(boardWin) * 2 / 3;
		interface_printxCenter(boardWin, LINES - 12, mid_col, info);
		int y = LINES - 19;
		char *buf = calloc(1024, sizeof(char));
		int col = getmaxx(boardWin) * 2.0 / 3.0;
		snprintf(buf, 1024, "%s (%s)", game->avatars[6]->player->username, game->avatars[6]->role == SHERIFF || game->avatars[6]->isDead ? print_role(game->avatars[6]->role) : "Unkown");
		interface_printxCenter(boardWin, y, col, buf);
		snprintf(buf, 1024, "%s",game->avatars[6]->character->name);
		interface_printxCenter(boardWin, y+1, col, buf);
		snprintf(buf, 1024, "HP(%d/%d)", game->avatars[6]->hp, game->avatars[6]->hp_max);
		interface_printxCenter(boardWin, y+2, col, buf);
	}

	mvwprintw(boardWin, 0, 2, "%s's Turn", username);
	wmove(boardWin, 1, 1);
	refresh();
	wrefresh(boardWin);
	return;
}

void interface_draw(char *username, Game *game) {
	interface_drawBoard(username, game);
	interface_drawInput(game->avatars[0], false, false, false, false);
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

void interface_drawInfo(Game *game) {
	int yMax, xMax;
	getmaxyx(stdscr, yMax, xMax);
	WINDOW *infoWin = newwin(yMax, xMax, 0, 0);

	init_pair(1, COLOR_BLACK, COLOR_GREEN);
	wattron(infoWin, COLOR_PAIR(1)); \
	interface_printCenter(infoWin, 0, "---Info---");
	wattroff(infoWin, COLOR_PAIR(1)); \
	int row = 0;
	for(int i = 0; i < game->numAvatar; i++) {
		wattron(infoWin, COLOR_PAIR(1)); \
		mvwprintw(infoWin, row + 1, 1, "%s", game->avatars[i]->character->name);
		wattroff(infoWin, COLOR_PAIR(1)); \
		row++;
		char *trav = game->avatars[i]->character->intro;
		while(1) {
			mvwprintw(infoWin, row + 1, 1, "%.*s", xMax - 2, trav);
			if(strlen(trav) <= xMax - 2) break;
			trav += xMax - 2;
			row++;
		}
		row++;
	}

	row++;
	init_pair(2, COLOR_BLACK, COLOR_YELLOW);
	wattron(infoWin, COLOR_PAIR(2)); \
	mvwprintw(infoWin, row + 1, 1, "---CARD---");
	wattroff(infoWin, COLOR_PAIR(2)); \
	row++;

	char *buffer = calloc(2048, sizeof(char));
	for(int i = 0; i < game->avatars[0]->cards_size; i++) {
		snprintf(buffer, 2048, "%s - %s", game->avatars[0]->cards[i]->name, game->avatars[0]->cards[i]->intro);
		char *trav = buffer;
		while(1) {
			mvwprintw(infoWin, row + 1, 1, "%.*s", xMax - 2, trav);
			if(strlen(trav) <= xMax - 2) break;
			trav += xMax - 2;
			row++;
		}
		row++;
	}

	interface_printCenter(infoWin, yMax - 2, " Press any key to exit info...");

	wrefresh(infoWin);
	refresh();

	getch();
	interface_draw(curAvatar->player->username, game);
	return;
}
