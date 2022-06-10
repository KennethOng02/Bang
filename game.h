#pragma once

#include <stdlib.h>
#include <assert.h>
#include "mysplit.h"
#include "card.h"

#define SWAP(A,B,TYPE) 	{ \
						TYPE tmp = A; \
						A = B; \
						B = tmp; \
						}


# define SHUFFLE(ARR,ARR_SIZE,ARR_TYPE) { \
	for ( int i=0; i<ARR_SIZE; i++ ) { \
		int tarIdx = rand() % ARR_SIZE; \
		SWAP(ARR[i], ARR[tarIdx], ARR_TYPE); \
	} \
}

Card **buildDeck() {
	FILE *pfile = fopen("card.txt", "r");
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
	return card;
}

void printDeck(Card ** deck, int deck_size) {
	for ( int i=0; i<deck_size; i++ ) {
		printf("Card %d\n", i+1);
		printf("	Name: %s\n", deck[i]->name);
		printf("	Type: %d\n", deck[i]->type);
		printf("	Suit: %d\n", deck[i]->suit);
	}
}

Card ** genDeck(int deck_size) {
	
	Card ** deck = buildDeck(); // initiate deck according "cards.txt";

	SHUFFLE(deck, deck_size, Card *);

	return deck;
}
