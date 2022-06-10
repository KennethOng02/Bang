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
	SHUFFLE(character_deck, character_deck_size, Character *);

	int players_size = 4;
	Player **players = malloc(players_size * sizeof(Player *));
	Role * roles = genRoles(players_size);
	for ( int i=0; i<players_size; i++ ) {
		players[i] = Player_init( character_deck[i], roles[i] );
	}




	return 0;
}
