#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "debug.h"
#include "mylib.h"
#include "avatar.h"
#include "deck.h"
#include "card.h"
#include "game.h"

Game *Game_init(int numAvatar) {

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
	Role *roles = genRoles(numAvatar);
	new->avatars = malloc(new->numAvatar * sizeof(Avatar *));
	Character **character_deck = Deck_genCharacter(CHARACTER_SIZE);
	for(int i = 0; i < numAvatar; i++ ) {
		new->avatars[i] = Avatar_init(i, character_deck[i], roles[i]);
		for ( int _=0; _<new->avatars[i]->hp; _++ ) {
			Avatar_draw(new->avatars[i], new);
		}
	}
	
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
}

void Game_run(Game *this) {
	int curIdx;
	for ( curIdx = 0; curIdx<this->numAvatar; curIdx++ ) {
		if ( this->avatars[curIdx]->role == SHERIFF ) {
			break;
		}
	}
	if ( curIdx >= this->numAvatar ) {
		ERROR_PRINT("No SHERIFF in this game.\n");
	}
	while ( 1 ) {
		Avatar_onTurn(this->avatars[curIdx], this);
		curIdx = (curIdx + 1) % this->numAvatar;
		// isDead?
		// game over?
	}
}
