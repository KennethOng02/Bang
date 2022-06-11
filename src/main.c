#include <stdio.h>
#include <stdlib.h>
#include "card.h"
#include "character.h"
#include "game.h"

#define DECK_SIZE 80
#define CHARACTER_SIZE 16
#define PLAYER_SIZE 4

int main(int argc, char ** argv) {
	puts("---Welcome to BANG!---");
	Card **deck = genDeck(DECK_SIZE);
	Character **character_deck = genCharacterDeck(CHARACTER_SIZE);
	Player **players = malloc(PLAYER_SIZE * sizeof(Player *));
	Role *roles = genRoles(PLAYER_SIZE);
	for(int i = 0; i < PLAYER_SIZE; i++ )
		players[i] = Player_init(character_deck[i], roles[i]);




	for(int i = 0; i < DECK_SIZE; i++)
		Card_free(deck[i]);
	free(deck);
	for(int i = 0; i < CHARACTER_SIZE; i++)
		Character_free(character_deck[i]);
	free(character_deck);
	for(int i = 0; i < PLAYER_SIZE; i++)
		Player_free(players[i]);
	free(players);
	free(roles);
	puts("---Exit BANG---");
	return 0;
}
