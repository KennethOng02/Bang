#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "debug.h"
#include "mylib.h"
#include "avatar.h"
#include "deck.h"
#include "card.h"
#include "game.h"
#include "player.h"
#include "interface.h"

static Game * game;

void Game_init(int numPlayer) {

	interface_welcome();

	game = malloc(sizeof(Game));
	
	// initiate Card *deck[DECK_SIZE]
	game->deck = Deck_gen(DECK_SIZE);

	// initiate Card *discardPile[DECK_SIZE] = {NULL}
	game->discardPile = calloc(DECK_SIZE, sizeof(Card *));
	for(int i = 0; i < DECK_SIZE; i++) {
		game->discardPile = Deck_init(DECK_SIZE);
	}

	Character **character_deck = Deck_genCharacter(CHARACTER_SIZE);
	// initiate Avatar *avatars[numPlayer]
	game->numPlayer = numPlayer;
	game->numAvailablePlayer = numPlayer;
	Role *roles = genRoles(numPlayer);
	game->players = malloc(game->numPlayer * sizeof(Player *));
	game->avatars = malloc(game->numPlayer * sizeof(Avatar *));
	for(int i = 0; i < numPlayer; i++ ) {
		// Initiate player and avatar
		game->players[i] = Player_init();
		game->avatars[i] = Avatar_init(i, character_deck[i], roles[i]);
		game->players[i]->avatar = game->avatars[i];
		game->avatars[i]->player = game->players[i];
		for ( int _=0; _<game->avatars[i]->hp; _++ ) {
			Avatar_draw(game->avatars[i], game);
		}
		DEBUG_PRINT("Finish initiation of player %s with avatar %d\n", game->players[i]->username, game->avatars[i]->id);
	}

	DEBUG_PRINT("Done Game_init\n");
	
	for(int i = 0; i < CHARACTER_SIZE; i++)
		Character_free(character_deck[i]);
	free(character_deck);
	free(roles);
}

void Game_free() {

	for ( int i=0; i<game->numPlayer; i++ ) {
		Avatar_free(game->avatars[i]);
		Player_free(game->players[i]);
	}
	free(game->players);
	free(game->avatars);

	Deck_free(game->deck, DECK_SIZE);

	Deck_free(game->discardPile, DECK_SIZE);

	free(game);
	
	DEBUG_PRINT("Game_free Done !\n");
	return;
}

void Game_run() {
	Avatar *sheriff = NULL;
	for ( int i=0; i<game->numPlayer; i++ ) {
		if ( game->avatars[i]->role == SHERIFF ) {
			sheriff = game->avatars[i];
			break;
		}
	}

	if ( sheriff == NULL ) {
		ERROR_PRINT("No SHERIFF in game game.\n");
	}
	Avatar *curAvatar = sheriff;

	DEBUG_PRINT("Stating game loop\n");
	while ( 1 ) {
		Avatar_onTurn(curAvatar, game);
		DEBUG_PRINT("Avatar %d's turn finish.\n", curAvatar->id);
		curAvatar = Game_nextAvailableAvatar(curAvatar);
	}
}

void Game_exit() {
	puts(GRN"---Exit BANG---"reset);
	Game_free();
	exit(0);
	return;
}

int Game_findIndex(Avatar *avatar)	{
	for ( int i=0; i<game->numPlayer; i++ ) {
		if ( game->avatars[i]->id == avatar->id ) {
			return i;
		}
	}
	return -1;
}

Avatar *Game_nextAvailableAvatar(Avatar *avatar) {
	int idx = Game_findIndex(avatar);
	if ( idx == -1 ) {
		ERROR_PRINT("Avatar %d not in game game.\n", avatar->id);
	}
	do {
		idx = (idx+1) % game->numPlayer;
	} while ( game->avatars[idx]->isDead );
	return game->avatars[idx];
}

void Game_checkWin() {
	Avatar *sheriff = NULL;
	bool teamSheriff = false;
	bool teamOutlaw = false;
	bool teamRenegade = false;
	for ( int i=0; i<game->numPlayer; i++ ) {
		if ( game->avatars[i]->role == SHERIFF ) {
			sheriff = game->avatars[i];
		}
		if ( !game->avatars[i]->isDead ) {
			switch ( game->avatars[i]->role ) {
			case SHERIFF:
			case DEPUTY:
				teamSheriff = true;
				break;
			case OUTLAW:
				teamOutlaw = true;
				break;
			case RENEGADE:
				teamRenegade = true;
				break;
			default:
				ERROR_PRINT("Unknown role.\n");
			}
		}
	}
	if ( sheriff->isDead ) {
		if ( teamOutlaw ) {
			printf("Outlaws win!\n");
			Game_exit();
		} else if ( teamRenegade && !teamSheriff ) {
			printf("Renegade wins!\n");
			Game_exit();
		}
	}
	if ( !teamOutlaw && !teamRenegade ) {
		printf("Sheriff wins!\n");
		Game_exit();
	}
}
