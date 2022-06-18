#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#include "game.h"
#include "player.h"
#include "print.h"

#define PLAYER_SIZE 4

int main(int argc, char ** argv) {

	Game_init(PLAYER_SIZE);

	Game_run();

	DEBUG_PRINT("HERE1.\n");
	Game_exit();

	return 0;
}
