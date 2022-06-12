#pragma once

typedef struct character Character;
typedef struct equipment Equipment;
typedef enum role Role;
typedef struct avatar Avatar;

#include "card.h"
#include "game.h"

struct character {
	char *name;
	int hp;
	char *intro;
};

Character *Character_init(const char *name, const int hp, const char *intro);

void Character_free(Character *this);


struct equipment {
	Card *gun;
	Card *armour;
	Card *horsePlus;
	Card *horseMinus;
	Card *jail;
	Card *bomb;
};

Equipment * Equipment_init();

void Equipment_free(Equipment *this);


enum role {
	SHERIFF,
	DEPUTY,
	OUTLAW,
	RENEGADE,
	ROLE_MAX
};


#define CARDS_SIZE_MAX 80

struct avatar {
	int			hp;
	Character	*character;
	Role		role;
	int			cards_size;
	Card 		**cards;
	Equipment	*equipment;
	int 		distancePlus;
	int			distanceMinus;
};

Avatar *Avatar_init(Character *character, Role role);

void Avatar_free(Avatar *this);

void Avatar_onTurn(Avatar *this, Game *game);
void Avatar_onJudge(Avatar *this, Game *game);
void Avatar_onDraw(Avatar *this, Game *game);
void Avatar_onPlay(Avatar *this, Game *game);
void Avatar_onDump(Avatar *this, Game *game);
void Avatar_onReact(Avatar *this, Game *game);
void Avatar_onDead(Avatar *this, Game *game);
void Avatar_onHurt(Avatar *this, Game *game);
