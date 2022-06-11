#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "player.h"
#include "card.h"
#include "character.h"
#include "equipment.h"

Player *Player_init(Character *character, Role role) {
	Player *new = malloc(sizeof(Player));

	new->character = Character_init(character->name, character->hp, character->intro);

	new->hp = new->character->hp;
	new->role = role;
	if(new->role == SHERIFF) {
		new->hp++;
	}
	new->card_bufSize = 10;
	new->card_size = 0;
	new->card = malloc(new->card_bufSize * sizeof(Card *));

	new->equipment = Equipment_init();
	new->distancePlus = 0;
	new->distanceMinus = 0;

	if(strcmp(new->character->name, "Paul Regret") == 0)
		new->distancePlus++;

	if(strcmp(new->character->name, "Rose Doolan") == 0)
		new->distanceMinus++;

	return new;
}

void Player_free(Player *this) {
	Character_free(this->character);
	for ( int i=0; i<this->card_size; i++ ) {
		Card_free(this->card[i]);
	}
	free(this->card);
	Equipment_free(this->equipment);
	free(this);
}
