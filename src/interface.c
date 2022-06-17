#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <ncurses.h>
#include "interface.h"
#include "player.h"
#include "game.h"
#include "card.h"
#include "debug.h"
#include "mylib.h"
#include "ANSI-color-codes.h"

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
	attron(A_STANDOUT | A_BOLD | A_REVERSE);
	interface_printCenter(stdscr, vmid - 4, " ____________________   __________ ");
	interface_printCenter(stdscr, vmid - 3, " ___  __ )__    |__  | / /_  ____/ ");
	interface_printCenter(stdscr, vmid - 2, " __  __  |_  /| |_   |/ /_  / __   ");
	interface_printCenter(stdscr, vmid - 1, " _  /_/ /_  ___ |  /|  / / /_/ /   ");
	interface_printCenter(stdscr, vmid,     " /_____/ /_/  |_/_/ |_/  \\____/    ");
	interface_printCenter(stdscr, vmid + 2, " COPYRIGHT to Din, Brian, Kenneth  ");
	attroff(A_STANDOUT | A_BOLD | A_REVERSE);
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
	char *name = calloc(32, sizeof(char));

	while(1) {
		mvwprintw(inputNameWin, 1, 2, "Player Name: ");
		refresh();
		wrefresh(inputNameWin);

		/* if(wgetnstr(inputNameWin, name, 32) == ERR) { */
		/* 	clearerr(stdin); */
		/* 	continue; */
		/* } */
		wgetstr(inputNameWin, name);

		while(isspace((unsigned char)*name)) name++;
		
		char *end = name + strlen(name) - 1;
		while(end > name && isspace((unsigned char)*end)) end--;
		end[1] = '\0';

		if(*name != 0) {
			break;
		}
		WARNING_PRINT("Empty Username.\n");
	}

	delwin(inputNameWin);
	redrawwin(stdscr);
	refresh();

	return name;
}

void interface_printCards(WINDOW *win, Card **cards, int cards_size) {
	wmove(win, 1, 1);
	for ( int i=0; i<cards_size; i++ ) {
		wprintw(win, "(%d)", i+1);
		if ( cards[i] ) {
			wprintw(win, "%s ", cards[i]->name);
		} else {
			wprintw(win, "UNKNOWN");
		}
	}
	moveCurDown(inputWin);
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

int *interface_choose(Player *this, Game *game, Card **cards, int cards_size, int n, char *msg, bool notChoose) {
	bool wanted[cards_size];
	bool done = false;
	int *choices = malloc(n * sizeof(int));

	int bufSize = 1024;
	char buffer[bufSize];

	while ( !done ) {
		
		memset(wanted, false, cards_size * sizeof(bool));

		wprintw(messgWin, "%s", msg);
		moveCurDown(messgWin);

		wrefresh(messgWin);
		interface_printCards(inputWin, cards, cards_size);

		/* scanw("%*[ \t\n]"); */
		
		/* if ( wgetnstr(stdscr, buffer, bufSize) == ERR ) { */
		/* 	clearerr(stdin); */
		/* 	continue; */
		/* } */
		wgetstr(inputWin, buffer);

		if ( strcmp(buffer, "q") == 0 ) {
			if(interface_yesOrNo(inputWin)) {
				Game_exit(game);
			}
			continue;
		}

		if ( notChoose && strcmp(buffer, "0") == 0 ) {
			free(choices);
			return NULL;
		}

		char **idxs;
		int idxs_size;

		mystrsplit(&idxs, &idxs_size, buffer, NULL);

		done = true;
		// Check if input is valid
		if ( idxs_size != n ) {
			WARNING_PRINT("Please enter %d integer(s).\n", n);
			done = false;
		}
		for ( int i=0; i<idxs_size; i++ ) {
			int idx;
			char *endptr;
			idx = strtol(idxs[i], &endptr, 10);
			if ( *endptr != '\0' ) {
				WARNING_PRINT("Please enter integer(s) only.\n");
				done = false;
			} else if ( idx <= 0 || idx > cards_size ) {
				WARNING_PRINT("Please enter integer(s) between [1, %d].\n", cards_size);
				done = false;
			} else if ( wanted[idx-1] ) {
				WARNING_PRINT("Choices duplicated.\n");
				done = false;
			}
			wanted[idx-1] = true;
		}

		for ( int i=0; i<idxs_size; i++ ) {
			free(idxs[i]);
		}
		free(idxs);

		if ( !done ) continue;

		int counter = 0;
		wprintw(boardWin, "Your choice(s) is:\n");
		for ( int i=0; i<cards_size; i++ ) {
			if ( wanted[i] && cards[i]) {
				wprintw(boardWin, "	%d. %s\n", counter+1, cards[i]->name);
				choices[counter++] = i;
			}else if ( wanted[i] ) {
				wprintw(boardWin, "	NO.%d card\n",i+1);
				choices[counter++] = i;
			}
		}
		interface_erase();
		interface_draw(this->username, game);
		refresh();
		if ( interface_yesOrNo(inputWin) ) {
			free(msg);
			return choices;
		} else {
			done = false;
		}
	}
	ERROR_PRINT("Unknown error.\n");
	return NULL;
}

int *interface_chooseTake(Player *this, Game *game, Card **cards, int cards_size, int n) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);
	/* snprintf(buffer, bufSize, "Please choose %d cards from following list.", n); */
	interface_erase();
	interface_draw(this->username, game);
	mvwprintw(inputWin, 2, 1, "Please choose %d cards from following list.", n);
	wrefresh(inputWin);
	return interface_choose(this, game, cards, cards_size, n, buffer, false);
}

int *interface_chooseDrop(Player *this, Game *game, Card **cards, int cards_size, int n) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);
	/* snprintf(buffer, bufSize, "Please choose %d card(s) to drop.", n); */
	wprintw(inputWin, "Please choose %d card(s) to drop.", n);
	wrefresh(inputWin);
	return interface_choose(this, game, cards, cards_size, n, buffer, false);
}

int interface_selectUse(Player *this, Game *game, Card **cards, int cards_size) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);
	/* snprintf(buffer, bufSize, "Please choose a card to use."); */
	interface_erase();
	interface_draw(this->username, game);
	wprintw(inputWin, "Please choose a card to use");
	wrefresh(inputWin);
	int *ret = interface_choose(this, game, cards, cards_size, 1, buffer, true);
	if ( ret == NULL ) {
		return -1;
	}
	return ret[0];
}

Player *interface_selectTarget(Player *this, Game *game) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);
	/* snprintf(buffer, bufSize, "Please choose which player as target."); */
	interface_erase();
	interface_draw(this->username, game);
	wmove(inputWin, 1, 1);
	wprintw(inputWin, "Please choose which player as target.");
	wmove(inputWin, 2, 1);
	int counter = 1;
	int y, x;
	for(int i = 0; i < game->numAvatar; i++) {
		if ( game->avatars[i]->isDead ) {
			continue;
		}
		wprintw(inputWin, "%d) Player %d (%s) %s", counter, game->avatars[i]->id, game->avatars[i]->player->username, game->avatars[i]->role == SHERIFF ? "(SHERIFF)" : "");
		getyx(inputWin, y, x);
		wmove(inputWin, y + 1, 1);
		counter++;
	}
	wrefresh(inputWin);
	int choice;
	while(1) {
		wprintw(inputWin, "Choice: ");
		refresh();
		if ( wscanw(inputWin, "%d", &choice) != 1 ) {
			WARNING_PRINT("Please enter an integer.\n");
			continue;
		}
		if(choice < 1 || choice > game->numAvailableAvatar) {
			WARNING_PRINT("Please input within range 1 to %d\n", game->numAvailableAvatar);
			continue;
		}
		wrefresh(inputWin);
		break;
	}
	return game->avatars[choice-1]->player;
}

int interface_selectReact(Player *this, Game *game, Card **cards, int cards_size) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);
	/* snprintf(buffer, bufSize, "Please choose a card to respond."); */
	wprintw(inputWin, "Please choose a card to respond.");
	wrefresh(inputWin);
	int *ret = interface_choose(this, game, cards, cards_size, 1, buffer, true);
	if ( ret != NULL ) {
		return ret[0];
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

void interface_playerInfo(Game *gametmp, Player* self) {
	Game *game = Game_queryInfo(self);
	Avatar *selfAvatar;
	for ( int i=0; i<game->numAvatar; i++ ) {
		if ( game->avatars[i]->id == self->id ) {
			selfAvatar = game->avatars[i];
		}
	}
	Avatar **avatars_list = game->avatars;
	printw("%d\n", game->numAvatar);
	printw("---Player Info---\n");	
	for(int i = 0; i < game->numAvatar; i++) {
		printw("Player %d\n", avatars_list[i]->id);
		printw("	Username: %s\n", avatars_list[i]->player->username);
		if(avatars_list[i]->isDead) {
			printw(RED"	---DEAD---\n"reset);
			printw("	Role: %s\n", print_role(avatars_list[i]->role));
			printw("	Character: %s\n", avatars_list[i]->character->name);
		}else {

			printw("	Hp: %d\n", avatars_list[i]->hp);

			printw("	Role: " );
			if ( avatars_list[i]->role != UNKNOWN ) {
				printw("%s\n", print_role(avatars_list[i]->role));
			} else {
				printw("UNKNOWN\n");
			}

			printw("	Character: %s\n", avatars_list[i]->character->name);

			printw("	Card Num: %d\n", avatars_list[i]->cards_size);

			printw("	Equipment: \n");
			Equipment *eqi = avatars_list[i]->equipment;
			if( eqi->gun != NULL) printw("		Gun: %s\n", eqi->gun->name);
			if( eqi->armour != NULL) printw("		Armour: %s\n", eqi->armour->name);
			if( eqi->horseMinus != NULL) printw("		Horse: %s\n", eqi->horseMinus->name);
			if( eqi->horsePlus != NULL) printw("		Horse: %s\n",eqi->horsePlus->name);
			if( eqi->bomb != NULL) printw("		%s\n", eqi->bomb->name);
			if( eqi->jail != NULL) printw("		%s\n", eqi->jail->name);
			printw("	Distance: %d\n", Avatar_calcDist(game, selfAvatar, avatars_list[i]));
		}
	}
	refresh();
	Game_freeCopy(game);
	return;
}

void interface_drawMessg() {
	int yMax, xMax;
	getmaxyx(stdscr, yMax, xMax);

	messgWin = newwin(10, xMax / 3, yMax - 10, xMax / 3 * 2);
	box(messgWin, 0, 0);
	mvwprintw(messgWin, 0, 2, "Messages");

	wmove(messgWin, 1, 1);
	wrefresh(messgWin);
	return;
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

void interface_printCardVertical(WINDOW *win, int num, int start_row) {
	int center_col = getmaxx(win) / 2;
	int half_length = 5 * num / 2;
	int adjusted_col = center_col - half_length;
	for(int i = 0; i < num; i++) mvwprintw(win, start_row, adjusted_col + i * 6,     "+---+ ");
	for(int i = 0; i < num; i++) mvwprintw(win, start_row + 1, adjusted_col + i * 6, "|   | ");
	for(int i = 0; i < num; i++) mvwprintw(win, start_row + 2, adjusted_col + i * 6, "|   | ");
	for(int i = 0; i < num; i++) mvwprintw(win, start_row + 3, adjusted_col + i * 6, "+---+ ");
	return;
}

void interface_printCardHorizontal(WINDOW *win, int num, int start_col) {
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

void interface_printPlayerInfoHorizontal(WINDOW *win, Avatar *avatar, int y) {
	char info[1024];
	snprintf(info, 1024, "%s (%s) (%s) HP(%d)", avatar->player->username, avatar->role == SHERIFF ? "SHERIFF" : "UNKNOWN", avatar->character->name, avatar->hp);
	interface_printCenter(win, y, info);
	return;
}

void interface_printPlayerInfoVertical(WINDOW *win, Avatar *avatar, int x) {
	int yMax, xMax;
	getmaxyx(win, yMax, xMax);
	int y = yMax / 2 - avatar->cards_size / 2 * 3 - 4;
	char info[1024];
	/* snprintf(info, 1024, "%s (%s) (%s) HP(%d)", avatar->player->username, avatar->role == SHERIFF ? "SHERIFF" : "UNKNOWN", avatar->character->name, avatar->hp); */
	mvwprintw(win, y, x, "%s", avatar->player->username);
	mvwprintw(win, y + 1, x, "(%s) (%s)", avatar->role == SHERIFF ? "SHERIFF" : "UNKNOWN", avatar->character->name);
	mvwprintw(win, y + 2, x, "HP(%d)", avatar->hp);
	wmove(win, y + 3, x);

	return;
}

void interface_drawBoard(char *username, Game *game) {
	int yMax, xMax, yBoard, xBoard;
	getmaxyx(stdscr, yMax, xMax);

	boardWin = newwin(yMax - 10, xMax, 0, 0);
	getmaxyx(boardWin, yBoard, xBoard);
	box(boardWin, 0, 0);

	interface_printCardVertical(boardWin, 2, yBoard / 2);

	interface_printCardVertical(boardWin, game->avatars[0]->cards_size, LINES - 15);

	interface_printCardHorizontal(boardWin, game->avatars[1]->cards_size, 2);
	interface_printPlayerInfoVertical(boardWin, game->avatars[1], 2);

	interface_printCardVertical(boardWin, game->avatars[2]->cards_size, 2);
	interface_printPlayerInfoHorizontal(boardWin, game->avatars[2], 1);

	interface_printCardHorizontal(boardWin, game->avatars[3]->cards_size, COLS - 9);
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

void interface_erase() {
	wmove(inputWin, 1, 1);
	wclrtobot(inputWin);

	wmove(messgWin, 1, 1);
	wclrtobot(messgWin);

	return;
}
