#pragma once

typedef struct player Player;

#include "card.h"
#include "character.h"
#include "equipment.h"

typedef enum role Role;
enum role {
	SHERIFF,
	DEPUTY,
	OUTLAW,
	RENEGADE,
	ROLE_MAX
};

struct player {
	int			hp;
	Character	*character;
	Role		role;
	int			card_bufSize;
	int			card_size;
	Card 		**card;
	Equipment	*equipment;
	int 		distancePlus;
	int			distanceMinus;
};

Player *Player_init(Character *character, Role role);

void Player_free(Player *this);

void Player_onTurn(Player *this);
void Player_onJudge(Player *this);
void Player_onDraw(Player *this);
void Player_onPlay(Player *this);
void Player_onDump(Player *this);
void Player_onReact(Player *this);
void Player_onDead(Player *this);
void Player_onHurt(Player *this);
