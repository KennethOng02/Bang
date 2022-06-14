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
Game *Game_init(int numAvatar, Player **players) {

	Game *new = malloc(sizeof(Game));
	
	// initiate Card *deck[DECK_SIZE]
	new->deck = Deck_gen(DECK_SIZE);

	// initiate Card *discardPile[DECK_SIZE] = {NULL}
	new->discardPile = calloc(DECK_SIZE, sizeof(Card *));
	for(int i = 0; i < DECK_SIZE; i++) {
		new->discardPile = Deck_init(DECK_SIZE);
	}

	// initiate Avatar *avatars[numAvatar]
	new->numAvatar = numAvatar;
	new->remainAvatar = numAvatar;
	Role *roles = genRoles(numAvatar);
	new->avatars = malloc(new->numAvatar * sizeof(Avatar *));
	Character **character_deck = Deck_genCharacter(CHARACTER_SIZE);
	for(int i = 0; i < numAvatar; i++ ) {
		new->avatars[i] = Avatar_init(i, players[i], character_deck[i], roles[i]);
		for ( int _=0; _<new->avatars[i]->hp; _++ ) {
			Avatar_draw(new->avatars[i], new);
		}
	}

	DEBUG_PRINT("Done Game_init\n");
	
	for(int i = 0; i < CHARACTER_SIZE; i++)
		Character_free(character_deck[i]);
	free(character_deck);
	free(roles);

	return new;
}

void Game_free(Game *this) {

	for ( int i=0; i<this->numAvatar; i++ ) {
		Avatar_free(this->avatars[i]);
	}
	free(this->avatars);

	Deck_free(this->deck, DECK_SIZE);

	Deck_free(this->discardPile, DECK_SIZE);

	free(this);
	
	DEBUG_PRINT("Game_free Done !\n");
	return;
}

void Game_run(Game *this) {
	Avatar *curAvatar = NULL;
	for ( int i=0; i<this->numAvatar; i++ ) {
		if ( this->avatars[i]->role == SHERIFF ) {
			curAvatar = this->avatars[i];
			break;
		}
	}

	if ( curAvatar == NULL ) {
		ERROR_PRINT("No SHERIFF in this game.\n");
	}

	DEBUG_PRINT("Stating game loop\n");
	while ( 1 ) {
		Avatar_onTurn(curAvatar, this);
		if( this->deck->top + 1 <= 0){
			return;
		}
		curAvatar = Game_nextAliveAvatar(this, curAvatar);
		// isDead?
		// game over?
	}
}

int Game_findIndex(Game *this, Avatar *avatar)	{
	for ( int i=0; i<this->numAvatar; i++ ) {
		if ( this->avatars[i]->id == avatar->id ) {
			return i;
		}
	}
	return -1;
}

Avatar *Game_nextAliveAvatar(Game *this, Avatar *avatar) {
	int idx = Game_findIndex(this, avatar);
	if ( idx == -1 ) {
		ERROR_PRINT("Avatar %d not in this game.\n", avatar->id);
	}
	do {
		idx = (idx+1) % this->numAvatar;
	} while ( this->avatars[idx]->isDead );
	return this->avatars[ (idx+1) % this->numAvatar ];
}
