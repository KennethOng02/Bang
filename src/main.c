#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#include "game.h"
#include "player.h"

#define PLAYER_SIZE 4

int main(int argc, char ** argv) {

	Game *game = Game_init(PLAYER_SIZE);

	Game_run(game);

	Game_exit(game);

	return 0;
}
