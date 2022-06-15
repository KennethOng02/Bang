#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "player.h"

#define PLAYER_SIZE 4

int main(int argc, char ** argv) {

	Game_init(PLAYER_SIZE);

	Game_run();

	Game_exit();

	return 0;
}
