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

Card ** buildDeck(int deck_size) {
	Card ** deck = malloc(deck_size * sizeof(Card *));
	for ( int i=0; i<deck_size; i++ ) {
		deck[i] = malloc(sizeof(Card));
		deck[i]->type = i+1;
	}
	return deck;
}
	
Card ** genDeck(int deck_size) {
	
	Card ** deck = buildDeck(deck_size); // initiate deck according "cards.txt";
	
	SHUFFLE(deck, deck_size, Card *);
	
	return deck;
}

Card **buildCard() {
	FILE *pfile = fopen("card.txt", "r");
	assert(pfile);
	
	Card **card = calloc(80, sizeof(Card));
	char *buffer = calloc(1024, sizeof(char));
	while(fgets(buffer, 1024, pfile) != NULL) {
		char **line;
		int counter;
		mystrsplit(&line, &counter, buffer, ",");

		char *card_str = line[3];
		char **card_list;
		mystrsplit(&card_list, &counter, card_str, " ");
		for(int i = 0; i < counter; i++)
			card[i] = Card_init(line[0], strtod(line[1], NULL), strtod(card_list[i], NULL));
	}
	return card;
}
