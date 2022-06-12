#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "game.h"
#include "card.h"
#include "mylib.h"
#include "debug.h"

#define DECK_SIZE 80
#define CHARACTER_SIZE 16

Game *Game_init(int numAvatar) {

	Game *new = malloc(sizeof(Game));

	// initiate Card *deck[DECK_SIZE]
	new->deck = genDeck(DECK_SIZE);

	// initiate Card *discardPile[DECK_SIZE] = {NULL}
	new->discardPile = malloc(DECK_SIZE * sizeof(Card *));
	for ( int i=0; i<DECK_SIZE; i++ ) {
		new->discardPile[i] = NULL;
	}

	// initiate Avatar *avatars[numAvatar]
	new->numAvatar = numAvatar;
	Role *roles = genRoles(numAvatar);
	new->avatars = malloc(new->numAvatar * sizeof(Avatar *));
	Character **character_deck = genCharacterDeck(CHARACTER_SIZE);
	for(int i = 0; i < numAvatar; i++ ) {
		new->avatars[i] = Avatar_init(character_deck[i], roles[i]);
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

	for(int i = 0; i < DECK_SIZE; i++)
		if ( this->deck[i] ) Card_free(this->deck[i]);
	free(this->deck);

	for(int i = 0; i < DECK_SIZE; i++)
		if ( this->discardPile[i] ) Card_free(this->discardPile[i]);
	free(this->discardPile);

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












Card **buildDeck() {
	FILE *pfile = fopen("src/card.txt", "r");
	assert(pfile);
	
	int j = 0;
	Card **card = calloc(80, sizeof(Card *));
	char *buffer = calloc(1024, sizeof(char));
	while(fgets(buffer, 1024, pfile) != NULL) {
		char **line;
		int counter;
		mystrsplit(&line, &counter, buffer, ",");

		char *card_str = line[3];
		char **card_list;
		mystrsplit(&card_list, &counter, card_str, NULL);
		for(int i = 0; i < counter; i++)
			card[j++] = Card_init(line[0], strtod(line[1], NULL), strtod(card_list[i], NULL));
	}
	free(buffer);
	return card;
}

Card **genDeck(int deck_size) {
	Card **deck = buildDeck(); // initiate deck according "cards.txt";
	SHUFFLE(deck, deck_size, Card *);
	return deck;
}

Role *genRoles(int numAvatar) {
	if(numAvatar == 4) {
		Role * roles = malloc(4 * sizeof(Role));
		roles[0] = SHERIFF;
		roles[1] = roles[2] = OUTLAW;
		roles[3] = RENEGADE;
		SHUFFLE(roles, 4, Role);
		return roles;
	}
	return NULL;
}

Character **buildCharacterDeck() {
	FILE *pfile = fopen("src/character.txt", "r");
	assert(pfile);

	Character **character = calloc(16, sizeof(Character *));

	char *buffer = calloc(1024, sizeof(char));
	int i = 0;
	while(fgets(buffer, 1024, pfile) != NULL) {
		char **line;
		int counter;
		mystrsplit(&line, &counter, buffer, "\"");
		character[i++] = Character_init(line[1], strtod(line[3], NULL), line[5]);
	}
	return character;
}

Character **genCharacterDeck(int deck_size) {
	Character **deck = buildCharacterDeck(); 
	SHUFFLE(deck, deck_size, Character *);
	return deck;
}
