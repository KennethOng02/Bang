#include <stdio.h>
#include <stdlib.h>

#include "card.h"
#include "game.h"

int main(int argc, char ** argv) {
	int deck_size = 80;
	Card **deck = genDeck(deck_size);
	return 0;
}
