#pragma once

typedef struct game Game;

#include "card.h"
#include "avatar.h"

struct game {
	int numAvatar;
	Avatar ** avatars;
	Card ** deck;
	Card ** discardPile;
};

Game *Game_init(int numAvatar);
Card **buildDeck();
Card **genDeck(int deck_size);
Role *genRoles(int numAvatar);
Character **buildCharacterDeck();
Character **genCharacterDeck(int deck_size);


void Game_free(Game *this);


int Game_draw(Game *this, Avatar *avatar);


int Game_use(Game *this, Avatar *user, Avatar ** targets, Card * card);


int Game_drop(Game *this, Avatar *user, Card * card);



