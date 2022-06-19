#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "mylib.h"
#include "game.h"
#include "deck.h"
#include "card.h"
#include "cardid.h"
#include "print.h"

Card *Deck_draw(Deck *this) {
	Card *new = this->card_pile[this->top];
	this->card_pile[this->top--] = NULL;
	if ( this->top < 0 ) {
		DEBUG_PRINT("No card remain. reshuffling...\n");
		Game_reShuffle();
	}
	return new;
}

void Deck_put(Deck *this, Card *new) {
	(this->top)++;
	this->card_pile[this->top] = new;
	return;
}

Deck *Deck_init(int size) {
	Deck *new = calloc(1, sizeof(Deck));
	new->size = size;
	new->card_pile = calloc(size, sizeof(Card *));
	new->top = -1;
	return new;
}

Deck *Deck_copy(Deck *this) {
	Deck *new = malloc(sizeof(Deck));
	new->size = this->size;
	new->card_pile = calloc(new->size, sizeof(Card *));
	new->top = this->top;
	for ( int i=0; i<=this->top; i++ ) {
		new->card_pile[i] = Card_copy(this->card_pile[i]);
	}
	return new;
}

void Deck_free(Deck *this, int size) {
	for(int i = 0; i < size; i++)
		if ( this->card_pile[i] ) Card_free(this->card_pile[i]);
	free(this);
	//DEBUG_PRINT("Deck_free Done !\n");
	return;
}

Deck *Deck_build() {
	// build deck and generating 'cardid.h' according 'card.txt'
	FILE *pfile = fopen("src/card.txt", "r");
	assert(pfile);

	Deck *deck = Deck_init(DECK_SIZE);

	int lineNum = 0;

	int bufSize = 1024;
	char *buffer = calloc(bufSize, sizeof(char));
	while(fgets(buffer, bufSize, pfile) != NULL) {

		char **line;
		int counter;
		mystrsplit(&line, &counter, buffer, ",");

		if ( counter == 5 ) {

			char *card_str = line[4];
			char **card_list;
			int cardCounter;
			mystrsplit(&card_list, &cardCounter, card_str, NULL);

			if ( cardCounter != strtod(line[3], NULL) ) {
				ERROR_PRINT("number of card inconsistent with suit list in line %d\n", lineNum);
			}

			for(int i = 0; i < cardCounter; i++) {
				Card *new_card = Card_init(lineNum, line[0], strtol(line[2],NULL,10), strtod(card_list[i], NULL), Card_funArr[lineNum]);
				Deck_put(deck, new_card);
			}

			for ( int i=0; i<cardCounter; i++ ) {
				free(card_list[i]);
			}
			free(card_list);

		}

		lineNum++;

		for ( int i=0; i<counter; i++ ) {
			free(line[i]);
		}
		free(line);
	}
	fclose(pfile);
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
	Role *roles = malloc(numAvatar * sizeof(Role));
	if ( numAvatar >= 4 ) {
		roles[0] = SHERIFF;
		roles[1] = roles[2] = OUTLAW;
		roles[3] = RENEGADE;
	}
	if ( numAvatar >= 5 ) {
		roles[4] = DEPUTY;
	}
	if ( numAvatar >= 6 ) {
		roles[5] = OUTLAW;
	}
	if ( numAvatar >= 7 ) {
		roles[6] = SHERIFF;
	}
	SHUFFLE(roles, numAvatar, Role);
	DEBUG_PRINT("Finish shuffling roles\n");
	for ( int i=0; i<numAvatar; i++ ) {
		DEBUG_PRINT("i\n");
	}
	return roles;
}

Character **Deck_buildCharacter() {
	FILE *pfile = fopen("src/character.txt", "r");
	assert(pfile);

	Character **character = calloc(16, sizeof(Character *));

	char *buffer = calloc(1024, sizeof(char));
	int i = 0;
	int lineNum = 0;
	while(fgets(buffer, 1024, pfile) != NULL) {
		char **line;
		int counter;
		mystrsplit(&line, &counter, buffer, "\"");
		character[i++] = Character_init( lineNum, line[1], strtod(line[3], NULL), line[5]);
		lineNum++;
	}
	return character;
}

Character **Deck_genCharacter(int deck_size) {
	Character **deck = Deck_buildCharacter(); 
	SHUFFLE(deck, deck_size, Character *);
	return deck;
}
