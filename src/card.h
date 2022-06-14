#pragma once

#include <stdbool.h>

typedef struct card Card;

#include "avatar.h"
#include "game.h"

struct card {
	int id;	
	char *name;
	int suit; // [0 - 51] //[Spade, Heart, Diamond, Club]
	bool (* play)(Avatar * user, Avatar ** target);
};

Card *Card_init(const int id, const char *name, const int suit);

void Card_free(Card *this);

bool Card_playBANG(Avatar * user, Avatar * target, Game * game, Card * card);
bool Card_playXXX(Avatar * user, Avatar * target, Game * game, Card * card);
