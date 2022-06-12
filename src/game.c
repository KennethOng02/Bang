#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "game.h"
#include "card.h"
#include "deck.h"
#include "mylib.h"

Game *Game_init(int numAvatar) {

	Game *new = malloc(sizeof(Game));

	// initiate Avatar *avatars[numAvatar]
	new->numAvatar = numAvatar;
	Role *roles = genRoles(numAvatar);
	new->avatars = malloc(new->numAvatar * sizeof(Avatar *));
	Character **character_deck = Deck_genCharacter(CHARACTER_SIZE);
	for(int i = 0; i < numAvatar; i++ )
		new->avatars[i] = Avatar_init(character_deck[i], roles[i]);

	// initiate Card *deck[DECK_SIZE]
	new->deck = Deck_gen(DECK_SIZE);

	// initiate Card *discardPile[DECK_SIZE] = {NULL}
	new->discardPile = malloc(DECK_SIZE * sizeof(Card *));
	for(int i = 0; i < DECK_SIZE; i++) {
		new->discardPile = Deck_init(DECK_SIZE);
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
