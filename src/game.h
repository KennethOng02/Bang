#pragma once
#include "card.h"

Card **buildDeck();

void printDeck(Card **deck, int deck_size);

Card **genDeck(int deck_size);

Role *genRoles(int numPlayer);
