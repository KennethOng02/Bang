#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "card.h"
#include "character.h"
#include "game.h"

int main(int argc, char ** argv) {
	srand(time(0));
	int deck_size = 80;
	int character_deck_size = 16;
	Card **deck = genDeck(deck_size);
	Character **character_deck = genCharacterDeck(character_deck_size);
	printCharacterDeck(character_deck, character_deck_size);
	return 0;
}
