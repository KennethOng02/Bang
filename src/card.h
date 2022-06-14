#pragma once

#include <stdbool.h>

typedef struct card Card;

#include "avatar.h"
#include "game.h"

struct card {
	int id;	
	char *name;
	int dist; // {0, 1, 2, 3}
	int suit; // [0 - 51] //[Spade, Heart, Diamond, Club]
	bool (* play)(Avatar * user, Avatar ** target);
};

Card *Card_init(const int id, const char *name, const int dist, const int suit, int (*fun)(Avatar *, Avatar *, Game *, Card *));

void Card_free(Card *this);

bool play_CARD_BANG(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_MISS(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_GATLING(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_INDIANS(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_PANIC(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_BALOU(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_STAGECOACH(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_FARGO(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_STORE(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_BEER(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_SALOON(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_DUEL(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_BARREL(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_SCOPE(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_MUSTANG(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_VOLCANIC(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_SCHOFIELD(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_REMINGTON(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_CARABINE(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_WINCHEDTER(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_JAIL(Avatar * user, Avatar * target, Game * game, Card * card);
bool play_CARD_DYNAMITE(Avatar * user, Avatar * target, Game * game, Card * card);
