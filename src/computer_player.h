#pragma once

#include <stdbool.h>

#include "player.h"
#include "game.h"
#include "card.h"

char *computer_askName();

int *computer_choose(Player *this, Game *game, Card **cards, int cards_size, int n, char *msg, bool notChoose);

int *computer_chooseTake(Player *this, Game *game, Card **cards, int cards_size, int n);
// select n cards from cards[0..cards_size) to take
// return their indexes

int *computer_chooseDrop(Player *this, Game *game, Card **cards, int cards_size, int n);
// select n cards from cards[0..cards_size) to drop
// return their indexes

int computer_selectUse(Player *this, Game *game, Card **cards, int cards_size, Player **target);
// select 1 card from cards[0..cards_size) to use
// return its index

int computer_selectReact(Player *this, Game *game, Card **cards, int cards_size);
// select 1 card from cards[0..cards_size) to use to react

bool computer_useAbility(Player *this, Game *game);
// whether of not use character ability
