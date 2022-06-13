#pragma once

#include <stdbool.h>

#include "player.h"
#include "game.h"
#include "card.h"

int *interface_choose(Player *this, Game *game, Card **cards, int cards_size, int n, char *msg, bool notChoose);
bool interface_yesOrNo();

int *interface_chooseTake(Player *this, Game *game, Card **cards, int cards_size, int n);
// select n cards from cards[0..cards_size) to take
// return their indexes

int *interface_chooseDrop(Player *this, Game *game, Card **cards, int cards_size, int n);
// select n cards from cards[0..cards_size) to drop
// return their indexes

int interface_selectUse(Player *this, Game *game, Card **cards, int cards_size);
// select 1 card from cards[0..cards_size) to use
// return its index

int interface_selectReact(Player *this, Game *game, Card **cards, int cards_size);
// select 1 card from cards[0..cards_size) to use to react

bool interface_useAbility(Player *this, Game *game);
// whether of not use character ability


void interface_printCards(Card **cards, int cards_size);
