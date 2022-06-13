#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "player.h"

#define PLAYER_SIZE 4

int main(int argc, char ** argv) {

	Player **players = malloc(PLAYER_SIZE * sizeof(Player *));
	for ( int i=0; i<PLAYER_SIZE; i++ ) {
		players[i] = malloc(sizeof(Player));
	}

	puts("---Welcome to BANG!---");

	Game *game = Game_init(PLAYER_SIZE, players);

	Player_initWithoutMalloc(&players[0], true, "Rabin", game->avatars[0]);
	Player_initWithoutMalloc(&players[1], true, "Poo Poo", game->avatars[1]);
	Player_initWithoutMalloc(&players[2], true, "Loise", game->avatars[2]);
	Player_initWithoutMalloc(&players[3], true, "Duke", game->avatars[3]);

	Game_run(game);

	puts("---Exit BANG---");
	Game_free(game);
	return 0;
}
