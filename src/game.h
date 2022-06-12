#pragma once

typedef struct game Game;

#include "card.h"
#include "deck.h"
#include "avatar.h"

#define DECK_SIZE 80
#define CHARACTER_SIZE 16

struct game {
	int numAvatar;
	Avatar ** avatars;
	Deck * deck;
	Deck * discardPile;
};

Game *Game_init(int numAvatar);
Card **buildDeck();
Card **genDeck(int deck_size);
Role *genRoles(int numAvatar);
Character **buildCharacterDeck();
Character **genCharacterDeck(int deck_size);


void Game_free(Game *this);


Card **Game_draw(Game *this, int num);


int Game_use(Game *this, Avatar *user, Avatar ** targets, Card * card);


int Game_drop(Game *this, Avatar *user, Card * card);



