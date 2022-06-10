#pragma once
#include <stdlib.h>
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

typedef struct player Player;
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

Player *Player_init(Character *character, Role role) {
	Player *new = malloc(sizeof(Player));
	new->character = character;
	new->hp = new->character->hp;
	new->role = role;
	if ( new->role == SHERIFF ) {
		new->hp++;
	}
	new->card_bufSize = 10;
	new->card_size = 0;
	new->card = malloc(new->card_bufSize * sizeof(Card *));

	new->equipment = Equipment_init();
	new->distancePlus = 0;
	new->distanceMinus = 0;
	if ( strcmp(new->character->name, "Paul Regret") == 0 ) {
		new->distancePlus++;
	}
	if ( strcmp(new->character->name, "Rose Doolan") == 0 ) {
		new->distanceMinus++;
	}
	return new;
}

void Player_onTurn(Player *this);
void Player_onJudge(Player *this);
void Player_onDraw(Player *this);
void Player_onPlay(Player *this);
void Player_onDump(Player *this);
void Player_onReact(Player *this);
void Player_onDead(Player *this);
void Player_onHurt(Player *this);
