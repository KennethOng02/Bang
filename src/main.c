#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>

#include "game.h"
#include "player.h"
#include "print.h"

#define PLAYER_SIZE 4

int main(int argc, char ** argv) {

	srand(time(0));

	Game_init(PLAYER_SIZE);

	Game_run();

	Game_exit();

	return 0;
}
