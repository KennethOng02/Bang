#pragma once
#include <stdlib.h>
#include <assert.h>
#include "mylib.h"
#include "card.h"

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
	return card;
}

void printDeck(Card **deck, int deck_size) {
	for(int i = 0; i < deck_size; i++) {
		printf("Card %d\n", i+1);
		printf("	Name: %s\n", deck[i]->name);
		printf("	Type: %d\n", deck[i]->type);
		printf("	Suit: %d\n", deck[i]->suit);
	}
}

Card **genDeck(int deck_size) {
	Card **deck = buildDeck(); // initiate deck according "cards.txt";
	SHUFFLE(deck, deck_size, Card *);
	return deck;
}

Role *genRoles(int numPlayer) {
	if(numPlayer == 4) {
		Role * roles = malloc(4 * sizeof(Role));
		roles[0] = SHERIFF;
		roles[1] = roles[2] = OUTLAW;
		roles[3] = RENEGADE;
		SHUFFLE(roles, 4, Role);
		return roles;
	}
	return NULL;
}
