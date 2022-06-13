#include <stdio.h>
#include <stdlib.h>
#include "game.h"

#define PLAYER_SIZE 4

int main(int argc, char ** argv) {
	puts("---Welcome to BANG!---");
	Game *game = Game_init(PLAYER_SIZE);

	Game_run(game);

	puts("---Exit BANG---");
	Game_free(game);
	return 0;
}
