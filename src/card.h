#pragma once

typedef struct card Card;

#include "player.h"

struct card {
	char *name;
	int type; // CARDTYPE_HAND or CARDTYPE_EQUIPMENT
	int suit; // [0 - 51] //[Spade, Heart, Diamond, Club]
	int (* play)(Player * user, Player ** target);
};

Card *Card_init(const char *name, const int type, const int suit);

void Card_free(Card *this);

int Card_playBang(Player * user, Player ** target);
int Card_playXXX(Player * user, Player ** target);
