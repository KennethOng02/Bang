#pragma once

typedef struct game Game;

#include "card.h"
#include "deck.h"
#include "avatar.h"
#include "player.h"

#define DECK_SIZE 80
#define CHARACTER_SIZE 16

struct game {
	int numPlayer;
	int numAvailablePlayer;
	Player ** players;
	Avatar ** avatars;
	Deck * deck;
	Deck * discardPile;
};

Game *Game_init(int numPlayer);
Card **buildDeck();
Card **genDeck(int deck_size);
Role *genRoles(int numAvatar);
Character **buildCharacterDeck();
Character **genCharacterDeck(int deck_size);


void Game_free(Game *this);


void Game_run(Game *this);


void Game_exit(Game *this);

void Game_checkWin(Game *this);

int Game_findIndex(Game *this, Avatar *avatar);
Avatar *Game_nextAvailableAvatar(Game *this, Avatar *avatar);
