#pragma once

typedef struct deck Deck;

#include "avatar.h"

struct deck {
	int top;
	int size;
	Card ** card_pile;
};

Card *Deck_draw(Deck *this);

void Deck_put(Deck *this, Card *new);

Deck *Deck_init(int num);

Deck *Deck_copy(Deck *this);

void Deck_free(Deck *this, int size);

Deck *Deck_build();

Deck *Deck_gen(int deck_size);

Role *genRoles(int numAvatar);

Character **Deck_buildCharacter();

Character **Deck_genCharacter(int deck_size);
