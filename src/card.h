#pragma once

typedef struct card Card;

#include "avatar.h"

struct card {
	int id;	
	char *name;
	int suit; // [0 - 51] //[Spade, Heart, Diamond, Club]
	int (* play)(Avatar * user, Avatar ** target);
};

Card *Card_init(const int id, const char *name, const int suit);

void Card_free(Card *this);

int Card_playBang(Avatar * user, Avatar ** target);
int Card_playXXX(Avatar * user, Avatar ** target);
