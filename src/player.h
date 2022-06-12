#pragma once

#include <stdbool.h>

typedef struct player Player

struct player {
	bool isComputer;
}

Player * Player_init(bool);

void Player_free(Player *);


int *Player_sel(Player *this, Game *game, Card **cards, int cards_size, int n);
// select n cards from cards[0..cards_size)
// return their indexes

int *Player_selDrop(Player *this, Game *game, Card **cards, int cards_size, int n);
// select n cards to drop from cards[0..cards_size)
// return their indexes

int Player_selUse(Player *this, Game *game, Card **cards, int cards_size);
// select 1 card to use from cards[0..cards_size)
// return its index
