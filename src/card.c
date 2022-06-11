#include <stdlib.h>
#include <string.h>

#include "card.h"
#include "debug.h"
#include "player.h"

#define CARDTYPE_HAND 1
#define CARDTYPE_EQUIPMENT 2

Card *Card_init(const char *name, const int type, const int suit) {
	if(name == NULL) ERROR_PRINT("NULL pointer Mother Fucker !!\n");

	if(type != CARDTYPE_HAND && type != CARDTYPE_EQUIPMENT)
		ERROR_PRINT("type must within [1, 2]\n");

	if(suit < 0 && suit >= 52)
		ERROR_PRINT("suit must within [0, 51]\n");

	Card *new = malloc(sizeof(Card));

	new->name = malloc(strlen(name)+1);
	strcpy(new->name, name);
	new->type = type;
	new->suit = suit;
	new->play = NULL;
	return new;
}

void Card_free(Card *this) {
	free(this->name);
	free(this);
}

int Card_playBang(Player * user, Player ** target) {
	return 0;
}

int Card_playXXX(Player * user, Player ** target) {
	return 0;
}
