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

void Game_init(int numPlayer);


void Game_free();


void Game_run();


void Game_exit();


void Game_checkWin();
int Game_findIndex(Avatar *avatar);
Avatar *Game_nextAvailableAvatar(Avatar *avatar);
