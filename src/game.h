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
	int numAvailableAvatar;
	Avatar ** avatars;
	Deck * deck;
	Deck * discardPile;
};

void Game_init(int numPlayer);


Game *Game_copy(Game *this);


void Game_free();

void Game_freeCopy(Game *this);


void Game_run();


void Game_exit();


void Game_checkWin();
int Game_findIndex(Avatar *avatar);
void Game_reShuffle();
Avatar *Game_nextAvailableAvatar(Avatar *avatar);
Game *Game_queryInfo( Player *player );
