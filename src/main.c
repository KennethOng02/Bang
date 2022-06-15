#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#include "game.h"
#include "player.h"

#define PLAYER_SIZE 4

int main(int argc, char ** argv) {

	initscr();

	Game *game = Game_init(PLAYER_SIZE);

	Game_run(game);

	Game_exit(game);

	printw("hello");
	refresh();

	endwin();

	return 0;
}
