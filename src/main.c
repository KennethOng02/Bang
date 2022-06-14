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

	Game *game = Game_init(PLAYER_SIZE, players);

	Player_initWithoutMalloc(&players[0], false, game->avatars[0]);
	Player_initWithoutMalloc(&players[1], false, game->avatars[1]);
	Player_initWithoutMalloc(&players[2], false, game->avatars[2]);
	Player_initWithoutMalloc(&players[3], false, game->avatars[3]);

	Game_run(game);

	Game_exit(game);
	return 0;
}
