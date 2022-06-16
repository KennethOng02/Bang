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

// 救命，幫我改介面跟英文=w=
// 我沒有美術天分，英文又很破...

/* char *interface_askName() { */
/* 	char *name = calloc(32, sizeof(char)); */
/* 	while(1) { */
/* 		printf("Please input your name: "); */

/* 		if(fgets(name, 32, stdin) == NULL) { */
/* 			clearerr(stdin); */
/* 			printf("\n"); */
/* 			continue; */
/* 		} */
/* 		if ( name[strlen(name)-1] != '\n' ) { */
/* 			WARNING_PRINT("Username too long, at most 31 characters.\n"); */
/* 			char ch = 0; */
/* 			while ( ch != '\n' ) { */
/* 				scanf("%c", &ch); */
/* 			} */
/* 			continue; */
/* 		} */

/* 		while(isspace((unsigned char)*name)) name++; */
		
/* 		char *end = name + strlen(name) - 1; */
/* 		while(end > name && isspace((unsigned char)*end)) end--; */
/* 		end[1] = '\0'; */

/* 		if(*name != 0) { */
/* 			break; */
/* 		} */
/* 		WARNING_PRINT("Empty Username.\n"); */
/* 	} */
/* 	return name; */
/* } */
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

		if(wgetnstr(inputNameWin, name, 32) == ERR) {
			clearerr(stdin);
			continue;
		}

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

void interface_printCards(Card **cards, int cards_size) {
	for ( int i=0; i<cards_size; i++ ) {
		wprintw(boardWin, "%d) ", i+1);
		if ( cards[i] ) {
			wprintw(boardWin, "%s\n", cards[i]->name);
		} else {
			wprintw(boardWin, "UNKNOWN\n");
		}
	}
	wrefresh(boardWin);
	return;
}

/* bool interface_yesOrNo(void) { */
/* 	int bufSize = 10; */
/* 	char buffer[bufSize]; */
/* 	while ( true ) { */
/* 		printw("(y/n):"); */
/* 		if ( wgetnstr(stdscr, buffer, bufSize) == ERR ) { */
/* 			WARNING_PRINT("Please enter 'y' or 'n'.\n"); */
/* 			clearerr(stdin); */
/* 			continue; */
/* 		} */
/* 		if ( strcmp(buffer, "y") == 0 ) return true; */
/* 		if ( strcmp(buffer, "n") == 0 ) return false; */
/* 	} */
/* 	return false; */
/* } */
bool interface_yesOrNo(void) {
	int y, x;
	getyx(inputWin, y, x);
	mvwprintw(inputWin, 1, 1, "(y/n): ");
	wrefresh(inputWin);
	while(1) {
		char c = getch();
		switch(c) {
			case 'y':
				return true;
				break;
			case 'n':
				return false;
				break;
			default:
				clrtoeol();
				move(y, x+7);
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

		wprintw(messgWin, "%s\n", msg);
		if ( notChoose ) wprintw(messgWin, "Enter '0' to pass.\n");
		wprintw(messgWin, "Enter 'i' to check other game info.\n");
		wprintw(messgWin, "Enter 'q' to quit\n");
		refresh();
		interface_printCards(cards, cards_size);

		/* scanw("%*[ \t\n]"); */
		
		/* if ( wgetnstr(stdscr, buffer, bufSize) == ERR ) { */
		/* 	clearerr(stdin); */
		/* 	continue; */
		/* } */
		getstr(buffer);

		if ( strcmp(buffer, "i") == 0 ) {
			// TODO: Switch to game menu
			DEBUG_PRINT("This feature is fully implemented yet.\n");
			clear();
			interface_menu(game, this);
			continue;
		}

		if ( strcmp(buffer, "q") == 0 ) {
			if(interface_yesOrNo()) {
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
			if ( wanted[i] ) {
				wprintw(boardWin, "	%d. %s\n", counter+1, cards[i]->name);
				choices[counter++] = i;
			}
		}
		interface_erase();
		interface_draw(this->username, game);
		refresh();
		if ( interface_yesOrNo() ) {
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
	printw("Please choose %d cards from following list.", n);
	refresh();
	return interface_choose(this, game, cards, cards_size, n, buffer, false);
}

int *interface_chooseDrop(Player *this, Game *game, Card **cards, int cards_size, int n) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);
	/* snprintf(buffer, bufSize, "Please choose %d card(s) to drop.", n); */
	printw("Please choose %d card(s) to drop.", n);
	refresh();
	return interface_choose(this, game, cards, cards_size, n, buffer, false);
}

int interface_selectUse(Player *this, Game *game, Card **cards, int cards_size) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);
	/* snprintf(buffer, bufSize, "Please choose a card to use."); */
	printw("Please choose a card to use");
	refresh();
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
	printw("Please choose which player as target.");
	int counter = 1;
	for(int i = 0; i < game->numAvatar; i++) {
		if ( game->avatars[i]->isDead ) {
			continue;
		}
		printw("%d) Player %d (%s) %s\n", counter, game->avatars[i]->id, game->avatars[i]->player->username, game->avatars[i]->role == SHERIFF ? "(SHERIFF)" : "");
		counter++;
	}
	refresh();
	int choice;
	while(1) {
		printw("Choice: ");
		refresh();
		if ( scanw("%d", &choice) != 1 ) {
			WARNING_PRINT("Please enter an integer.\n");
			continue;
		}
		if(choice < 1 && choice >= game->numAvatar) {
			WARNING_PRINT("Please input within range 1 to %d\n", game->numAvatar);
			continue;
		}
		refresh();
		break;
	}
	return game->avatars[choice-1]->player;
}

int interface_selectReact(Player *this, Game *game, Card **cards, int cards_size) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);
	/* snprintf(buffer, bufSize, "Please choose a card to respond."); */
	printw("Please choose a card to respond.");
	refresh();
	int *ret = interface_choose(this, game, cards, cards_size, 1, buffer, true);
	if ( ret != NULL ) {
		return ret[0];
	}
	return -1;
}

bool interface_useAbility(Player *this, Game *game) {
	printw("Do you want to use your characteristic ability?\n");
	refresh();
	return interface_yesOrNo();
}

void interface_menu(Game *game, Player *self) {
	interface_playerInfo(game, self);
	/* interface_discardedPileInfo(game->avatars); */
	return;
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

	refresh();
	wrefresh(messgWin);
	return;
}

void interface_drawInput(Avatar *avatar) {
	int yMax, xMax;
	getmaxyx(stdscr, yMax, xMax);

	inputWin = newwin(10, xMax / 3 * 2, yMax - 10, 0);
	box(inputWin, 0, 0);
	mvwprintw(inputWin, 0, 2, "%s-(%s)-(%s)-HP(%d)", avatar->player->username, print_role(avatar->role), avatar->character->name, avatar->hp);
	mvwprintw(inputWin, 0, xMax - 25, "[0]pass--[q]quit");

	refresh();
	wrefresh(inputWin);
	return;
}

void interface_drawBoard(char *username) {
	int yMax, xMax, yBoard, xBoard;
	getmaxyx(stdscr, yMax, xMax);

	boardWin = newwin(yMax - 10, xMax, 0, 0);
	getmaxyx(boardWin, yBoard, xBoard);
	box(boardWin, 0, 0);

	mvwprintw(boardWin, 0, 2, "%s's Turn", username);
	refresh();
	wrefresh(boardWin);
	return;
}

void interface_draw(char *username, Game *game) {
	interface_drawBoard(username);
	interface_drawInput(game->avatars[0]);
	interface_drawMessg();
	refresh();
	return;
}

void interface_erase() {
	werase(boardWin);
	werase(inputWin);
	werase(messgWin);
	erase();
	return;
}
