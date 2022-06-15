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

Game *Game_init(int numPlayer) {

	interface_welcome();

	Game *new = malloc(sizeof(Game));
	
	// initiate Card *deck[DECK_SIZE]
	new->deck = Deck_gen(DECK_SIZE);

	// initiate Card *discardPile[DECK_SIZE] = {NULL}
	new->discardPile = calloc(DECK_SIZE, sizeof(Card *));
	for(int i = 0; i < DECK_SIZE; i++) {
		new->discardPile = Deck_init(DECK_SIZE);
	}

	Character **character_deck = Deck_genCharacter(CHARACTER_SIZE);
	// initiate Avatar *avatars[numPlayer]
	new->numPlayer = numPlayer;
	new->numAvailablePlayer = numPlayer;
	Role *roles = genRoles(numPlayer);
	new->players = malloc(new->numPlayer * sizeof(Player *));
	new->avatars = malloc(new->numPlayer * sizeof(Avatar *));
	for(int i = 0; i < numPlayer; i++ ) {
		// Initiate player and avatar
		new->players[i] = Player_init();
		new->avatars[i] = Avatar_init(i, character_deck[i], roles[i]);
		new->players[i]->avatar = new->avatars[i];
		new->avatars[i]->player = new->players[i];
		for ( int _=0; _<new->avatars[i]->hp; _++ ) {
			Avatar_draw(new->avatars[i], new);
		}
		DEBUG_PRINT("Finish initiation of player %s with avatar %d\n", new->players[i]->username, new->avatars[i]->id);
	}

	DEBUG_PRINT("Done Game_init\n");
	
	for(int i = 0; i < CHARACTER_SIZE; i++)
		Character_free(character_deck[i]);
	free(character_deck);
	free(roles);

	return new;
}

void Game_free(Game *this) {

	for ( int i=0; i<this->numPlayer; i++ ) {
		Avatar_free(this->avatars[i]);
		Player_free(this->players[i]);
	}
	free(this->players);
	free(this->avatars);

	Deck_free(this->deck, DECK_SIZE);

	Deck_free(this->discardPile, DECK_SIZE);

	free(this);
	
	DEBUG_PRINT("Game_free Done !\n");
	return;
}

void Game_run(Game *this) {
	Avatar *sheriff = NULL;
	for ( int i=0; i<this->numPlayer; i++ ) {
		if ( this->avatars[i]->role == SHERIFF ) {
			sheriff = this->avatars[i];
			break;
		}
	}

	if ( sheriff == NULL ) {
		ERROR_PRINT("No SHERIFF in this game.\n");
	}
	Avatar *curAvatar = sheriff;

	DEBUG_PRINT("Stating game loop\n");
	while ( 1 ) {
		Avatar_onTurn(curAvatar, this);
		DEBUG_PRINT("Avatar %d's turn finish.\n", curAvatar->id);

		curAvatar = Game_nextAvailableAvatar(this, curAvatar);
	}
}

void Game_exit(Game *game) {
	puts(GRN"---Exit BANG---"reset);
	Game_free(game);
	exit(0);
	return;
}

int Game_findIndex(Game *this, Avatar *avatar)	{
	for ( int i=0; i<this->numPlayer; i++ ) {
		if ( this->avatars[i]->id == avatar->id ) {
			return i;
		}
	}
	return -1;
}

Avatar *Game_nextAvailableAvatar(Game *this, Avatar *avatar) {
	int idx = Game_findIndex(this, avatar);
	if ( idx == -1 ) {
		ERROR_PRINT("Avatar %d not in this game.\n", avatar->id);
	}
	do {
		idx = (idx+1) % this->numPlayer;
	} while ( this->avatars[idx]->isDead );
	return this->avatars[idx];
}

void Game_checkWin(Game *this) {
	Avatar *sheriff = NULL;
	bool teamSheriff = false;
	bool teamOutlaw = false;
	bool teamRenegade = false;
	for ( int i=0; i<this->numPlayer; i++ ) {
		if ( this->avatars[i]->role == SHERIFF ) {
			sheriff = this->avatars[i];
		}
		if ( !this->avatars[i]->isDead ) {
			switch ( this->avatars[i]->role ) {
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
			Game_exit(this);
		} else if ( teamRenegade && !teamSheriff ) {
			printf("Renegade wins!\n");
			Game_exit(this);
		}
	}
	if ( !teamOutlaw && !teamRenegade ) {
		printf("Sheriff wins!\n");
		Game_exit(this);
	}
}
