#include <stdlib.h>
#include <string.h>

#include "avatar.h"
#include "debug.h"

Character *Character_init(const char *name, const int hp, const char *intro) {
	if(name == NULL || intro == NULL)
		ERROR_PRINT("NULL pointer Mother Fucker !!\n");

	if(hp < 0 && hp > 4)
		ERROR_PRINT("hp must be between 0 and 4");

	Character *new = malloc(sizeof(Character));

	new->name = malloc(strlen(name) + 1);
	strcpy(new->name, name);
	new->hp = hp;
	new->intro = malloc(strlen(intro)+1);
	strcpy(new->intro, intro);
	return new;
}

void Character_free(Character *this) {
	free(this->name);
	free(this->intro);
	free(this);
}


Equipment * Equipment_init() {
	Equipment * new = malloc(sizeof(Equipment));
	new->gun = NULL;
	new->armour = NULL;
	new->horsePlus = NULL;
	new->horseMinus = NULL;
	new->jail = NULL;
	new->bomb = NULL;
	return new;
}

void Equipment_free(Equipment *this) {
	if ( this->gun ) Card_free(this->gun);
	if ( this->armour ) Card_free(this->armour);
	if ( this->horsePlus ) Card_free(this->horsePlus);
	if ( this->horseMinus ) Card_free(this->horseMinus);
	if ( this->jail ) Card_free(this->jail);
	if ( this->bomb ) Card_free(this->bomb);
}


Avatar *Avatar_init(Character *character, Role role) {

	Avatar *new = malloc(sizeof(Avatar));

	new->character = Character_init(character->name, character->hp, character->intro);

	new->hp = new->character->hp;
	new->role = role;
	if(new->role == SHERIFF) {
		new->hp++;
	}
	new->cards_size = 0;
	new->cards = malloc(CARDS_SIZE_MAX * sizeof(Card *));

	new->equipment = Equipment_init();
	new->distancePlus = 0;
	new->distanceMinus = 0;

	if(strcmp(new->character->name, "Paul Regret") == 0)
		new->distancePlus++;

	if(strcmp(new->character->name, "Rose Doolan") == 0)
		new->distanceMinus++;

	return new;
}

void Avatar_free(Avatar *this) {
	Character_free(this->character);
	Equipment_free(this->equipment);
	for ( int i=0; i<this->cards_size; i++ ) {
		Card_free(this->cards[i]);
	}
	free(this->cards);
	free(this);
}

//void Avatar_onTurn(Avatar *this, Game *game) {
	

void Avatar_onJudge(Avatar *this, Game *game) {
	for ( Card *card = (Card *)this->equipment; card < (Card*)(this->equipment+sizeof(Equipment)); card++ ) {
		printf("HI\n");
	}
	return;
}

//void Avatar_onDraw(Avatar *this, Game *game);

//void Avatar_onPlay(Avatar *this, Game *game);
//void Avatar_onDump(Avatar *this, Game *game);
//void Avatar_onReact(Avatar *this, Game *game);
//void Avatar_onDead(Avatar *this, Game *game);
//void Avatar_onHurt(Avatar *this, Game *game);
