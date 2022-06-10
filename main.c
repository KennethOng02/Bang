#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "card.h"
#include "game.h"

int main(int argc, char ** argv) {
	srand(time(0));
	int deck_size = 80;
	Card **deck = genDeck(deck_size);
	printDeck(deck, deck_size);
	return 0;
}
