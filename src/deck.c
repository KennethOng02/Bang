#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "mylib.h"
#include "game.h"
#include "deck.h"
#include "card.h"
#include "debug.h"

Card *Deck_draw(Deck *this) {
	Card *new = this->card_pile[this->top];
	(this->top)--;
	return new;
}

void Deck_put(Deck *this, Card *new) {
	(this->top)++;
	this->card_pile[this->top] = new;
	return;
}

Deck *Deck_init(int size) {
	Deck *new = calloc(1, sizeof(Deck));
	new->card_pile = calloc(size, sizeof(Card *));
	new->top = -1;
	return new;
}

void Deck_free(Deck *this, int size) {
	for(int i = 0; i < size; i++)
		if ( this->card_pile[i] ) Card_free(this->card_pile[i]);
	free(this);
	return;
}

Deck *Deck_build() {
	FILE *pfile = fopen("src/card.txt", "r");
	assert(pfile);
	
	int j = 0;
	Deck *deck = Deck_init(DECK_SIZE);
	char *buffer = calloc(1024, sizeof(char));
	while(fgets(buffer, 1024, pfile) != NULL) {
		char **line;
		int counter;
		mystrsplit(&line, &counter, buffer, ",");

		char *card_str = line[3];
		char **card_list;
		mystrsplit(&card_list, &counter, card_str, NULL);
		for(int i = 0; i < counter; i++) {
			Card *new_card = Card_init(line[0], strtod(line[1], NULL), strtod(card_list[i], NULL));
			Deck_put(deck, new_card);
		}
	}
	free(buffer);
	return deck;
}

Deck *Deck_gen(int deck_size) {
	Deck *deck = Deck_build(); // initiate deck according "cards.txt";
	SHUFFLE(deck->card_pile, deck_size, Card *);
	DEBUG_PRINT("finish generating deck!\n");
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

Character **Deck_buildCharacter() {
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

Character **Deck_genCharacter(int deck_size) {
	Character **deck = Deck_buildCharacter(); 
	SHUFFLE(deck, deck_size, Character *);
	return deck;
}
