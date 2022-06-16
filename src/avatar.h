#pragma once

#include <stdbool.h>

typedef struct character Character;
typedef struct equipment Equipment; typedef enum role Role;
typedef struct avatar Avatar;

#include "card.h"
#include "game.h"
#include "player.h"

struct character {
	int id;
	char *name;
	int hp;
	char *intro;
};

Character *Character_init(int id, const char *name, const int hp, const char *intro);

Character * Character_copy(Character *this);

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

Equipment *Equipment_copy(Equipment *this);

void Equipment_free(Equipment *this);


#define UNKNOWN 1000

enum role {
	SHERIFF,
	DEPUTY,
	OUTLAW,
	RENEGADE,
	ROLE_MAX
};


#define CARDS_SIZE_MAX 80

struct avatar {
	int			id;
	bool		isDead;
	Player		*player;
	int			hp;
	int         hp_max;
	Character	*character;
	Role		role;
	int			cards_size;
	Card 		**cards;
	Equipment	*equipment;
};

Avatar *Avatar_init(int id, Character *character, Role role);

Avatar *Avatar_copy(Avatar *this);

void Avatar_free(Avatar *this);

void Avatar_freeCopy(Avatar *this);

void Avatar_onTurn(Avatar *this, Game *game);
void Avatar_onJudge(Avatar *this, Game *game, bool *jailed);
void Avatar_onDraw(Avatar *this, Game *game);
void Avatar_onPlay(Avatar *this, Game *game);
void Avatar_onDump(Avatar *this, Game *game);
int Avatar_onReact(Avatar *this, Game *game, int card_id, Card* to_react);
int Avatar_judge(Avatar *this, Game *game, int card_id);
void Avatar_dead(Avatar *this, Game *game);
void Avatar_hurt(Avatar *this, Game *game, Avatar *attacker);
void Avatar_heal(Avatar *this, Game *game);
void Avatar_equip(Avatar *this, Game *game, Card *card);
Card *Avatar_unequip(Avatar *this, Game *game, Card **card);
void Avatar_draw(Avatar *this, Game *game);
int* Avatar_choose(Avatar *this, Game *game, Card **options , int size, int num);
void Avatar_get(Avatar *this, Game *game, Card *want);
Card *Avatar_taken(Avatar *this, Game *game,int index);
int Avatar_calcDist(Game *game, Avatar *this, Avatar *that);
int Avatar_calcVision(Avatar *this);

Card **Avatar_giveToChoose(Avatar *this, int *retSize);