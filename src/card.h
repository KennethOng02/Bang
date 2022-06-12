#pragma once

typedef struct card Card;

#include "avatar.h"

struct card {
	char *name;
	int type; // CARDTYPE_HAND or CARDTYPE_EQUIPMENT
	int suit; // [0 - 51] //[Spade, Heart, Diamond, Club]
	int (* play)(Avatar * user, Avatar ** target);
};

Card *Card_init(const char *name, const int type, const int suit);

void Card_free(Card *this);

int Card_playBang(Avatar * user, Avatar ** target);
int Card_playXXX(Avatar * user, Avatar ** target);
