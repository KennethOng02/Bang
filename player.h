#pragma once

#include "card.h"
#include "character.h"
#include "equipment.h"

typedef struct player Player;
struct player {
	int			hp;
	Character	*charater;
	int			card_size;
	Card		**card;
	Equipment	*equipment;
	int	       	distancePlus;
	int	       	distanceMinus;
};

void Player_onTurn(Player *this);
void Player_onJudge(Player *this);
void Player_onDraw(Player *this);
void Player_onPlay(Player *this);
void Player_onDump(Player *this);
void Player_onReact(Player *this);
void Player_onDead(Player *this);
void Player_onHurt(Player *this);
