#pragma once

typedef struct game Game;

#include "card.h"
#include "deck.h"
#include "avatar.h"
#include "player.h"

#define DECK_SIZE 80
#define CHARACTER_SIZE 16

struct game {
	int numAvatar;
	Avatar ** avatars;
	Deck * deck;
	Deck * discardPile;
};

Game *Game_init(int numAvatar, Player **players);
Card **buildDeck();
Card **genDeck(int deck_size);
Role *genRoles(int numAvatar);
Character **buildCharacterDeck();
Character **genCharacterDeck(int deck_size);


void Game_free(Game *this);


void Game_run(Game *this);


int Game_find_index(Game *this, Avatar *avatar);


