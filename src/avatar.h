#pragma once

#include <stdbool.h>

typedef struct character Character;
typedef struct equipment Equipment;
typedef enum role Role;
typedef struct avatar Avatar;

#include "card.h"
#include "game.h"
#include "player.h"

struct character {
	char *name;
	int hp;
	char *intro;
};

Character *Character_init(const char *name, const int hp, const char *intro);

void Character_free(Character *this);


struct equipment {
	// Only card pointers
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
	int			id;
	bool		isDead;
	Player		*player;
	int			hp;
	Character	*character;
	Role		role;
	int			cards_size;
	Card 		**cards;
	Equipment	*equipment;
	int 		distancePlus;
	int			distanceMinus;
};

Avatar *Avatar_init(int id, Character *character, Role role);

void Avatar_free(Avatar *this);

void Avatar_onTurn(Avatar *this, Game *game);
void Avatar_onJudge(Avatar *this, Game *game, bool *jailed);
void Avatar_onDraw(Avatar *this, Game *game);
void Avatar_onPlay(Avatar *this, Game *game);
void Avatar_onDump(Avatar *this, Game *game);
int Avatar_onReact(Avatar *this, Game *game, int card_id);
void Avatar_dead(Avatar *this, Game *game);
void Avatar_hurt(Avatar *this, Game *game, Avatar *attacker);
void Avatar_heal(Avatar *this, Game *game);
void Avatar_equip(Avatar *this, Game *game, Card *card);
Card *Avatar_unequip(Avatar *this, Game *game, Card **card);
void Avatar_draw(Avatar *this, Game *game);
int* Avatar_choose(Avatar *this, Game *game, Card **options , int size, int num);
Card *Avatar_taken(Avatar *this, Game *game,int index);
