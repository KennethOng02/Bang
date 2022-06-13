#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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


Avatar *Avatar_init(int id, Character *character, Role role) {

	Avatar *new = malloc(sizeof(Avatar));

	new->id = id;

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

void Avatar_onTurn(Avatar *this, Game *game)  {

	DEBUG_PRINT("Avatar %d's turn.\n", this->id);

	bool jailed = false;
	Avatar_onJudge(this, game, &jailed);
	if ( jailed ) return;

	Avatar_onDraw(this, game);

}
	

void Avatar_onJudge(Avatar *this, Game *game, bool *jailed) {

	// Lucky Duke

	if ( this->equipment->bomb != NULL ) {
	
		DEBUG_PRINT("Avatar %d judge for bomb.\n", this->id);

		Card *card = Deck_draw(game->deck);
		int suit = card->suit;
		Deck_put(game->discardPile, card);

		Card *bomb = Avatar_unequip(this, game, &(this->equipment->bomb));
		if ( suit >= 1 && suit <= 8 ) {
			// suit is between [Spade 2, Spade 9]

			Deck_put(game->discardPile, bomb);
			for ( int _=0; _<3; _++ ) Avatar_hurt(this, game);

		} else {
			// Find next avatar
			for ( int i=0; i<game->numAvatar; i++ ) {
				if ( game->avatars[i]->id == this->id ) {
					Avatar *nextAvatar = game->avatars[(i+1)%game->numAvatar];
					Avatar_equip(nextAvatar, game, bomb);
					break;
				}
				if ( i == game->numAvatar-1 ) ERROR_PRINT("Cannot find avatar %d in this game.\n", this->id);
			}
		}
		
	}
	if ( this->equipment->jail != NULL ) {

		DEBUG_PRINT("Avatar %d judge for Jail.\n", this->id);

		Card *card = Deck_draw(game->deck);
		int suit = card->suit;
		Deck_put(game->discardPile, card);

		if ( suit < 13 || suit >= 26 ) {
			// suit is not heart
			*jailed = true;
		}
	}
	return;
}

void Avatar_onDraw(Avatar *this, Game *game) {
	// Black Jack
	// Jesse Jones
	// Kit Carlson
	for ( int _=0; _<2; _++ ) Avatar_draw(this, game);
}

void Avatar_onPlay(Avatar *this, Game *game) {
}

void Avatar_onDump(Avatar *this, Game *game) {
	
}

//void Avatar_onReact(Avatar *this, Game *game);
void Avatar_dead(Avatar *this, Game *game){

}
void Avatar_hurt(Avatar *this, Game *game){
	this->hp -- ;
	// if(this->hp == 0)
}
void Avatar_heal(Avatar *this, Game *game){
	this->hp ++ ;
	return ;
}
void Avatar_equip(Avatar *this, Game *game, Card *card){
	// if( this->equipment->gun != NULL){
	// 	Card *trash = Avatar_unequip(this,game,&(this->equipment->gun));
	// 	Deck_put(game->discardPile,trash);
	// }
	
}
Card* Avatar_unequip(Avatar *this, Game *game, Card **card){
	Card *bye = *card;
	*card = NULL;
	return bye;
}
void Avatar_draw(Avatar *this, Game *game){
	this->cards_size ++;
	this->cards[this->cards_size - 1] = Deck_draw(game->deck);
	return;
}
int* Avatar_choose(Avatar *this, Game *game, Card **options , int size, int num){	
}
void Avatar_get(Avatar *this, Game *game, Card *want){
	this->cards_size ++;
	this->cards[this->cards_size - 1] = want;
	return;
}
Card* Avatar_taken(Avatar *this, Game *game, int index){
	Card *bye = this->cards[index];
	for( size_t i = index ; i < this->cards_size - 1 ; i++ ){
		this->cards[index] = this->cards[index + 1];
	}
	this->cards_size -- ;
	return bye;
}

