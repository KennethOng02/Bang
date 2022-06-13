#pragma once

#include <stdbool.h>

typedef struct player Player ;

#include "game.h"
#include "avatar.h"
#include "card.h"

struct player {
	bool isComputer;
	char *username;
	Avatar *avatar;
};

void Player_initWithoutMalloc(Player **player, bool isComputer, char *username, Avatar *avatar);

void Player_free(Player *);


int *Player_chooseTake(Player *this, Game *game, Card **cards, int cards_size, int n);
// select n cards from cards[0..cards_size) to take
// return their indexes

int *Player_chooseDrop(Player *this, Game *game, Card **cards, int cards_size, int n);
// select n cards from cards[0..cards_size) to drop
// return their indexes

int Player_selectUse(Player *this, Game *game, Card **cards, int cards_size);
// select 1 card from cards[0..cards_size) to use
// return its index

int Player_selectReact(Player *this, Game *game, Card **cards, int cards_size);
// select 1 card from cards[0..cards_size) to use to react

bool Player_useAbility(Player *this, Game *game);
// whether of not use character ability
