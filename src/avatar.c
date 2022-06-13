#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "avatar.h"
#include "debug.h"
#include "cardid.h"
#include "player.h"

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


Avatar *Avatar_init(int id, Player *player, Character *character, Role role) {

	Avatar *new = malloc(sizeof(Avatar));

	new->id = id;
	new->player = player;

	new->character = Character_init(character->name, character->hp, character->intro);

	new->hp = new->character->hp;
	new->role = role;
	if(new->role == SHERIFF) {
		DEBUG_PRINT("SHERIFF is avatar %d.\n", id);
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
	// Free players in main.c
	Character_free(this->character);
	Equipment_free(this->equipment);
	for ( int i=0; i<this->cards_size; i++ ) {
		Card_free(this->cards[i]);
	}
	free(this->cards);
	free(this);
	DEBUG_PRINT("Avatar_free Done !\n");
}

void Avatar_onTurn(Avatar *this, Game *game)  {

	DEBUG_PRINT("Avatar %d's turn.\n", this->id);

	bool jailed = false;
	Avatar_onJudge(this, game, &jailed);
	if ( jailed ) return;

	Avatar_onDraw(this, game);

	Avatar_onPlay(this, game);
	
	Avatar_onDump(this, game);
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
			int index = Game_find_index( game, this );
			if ( index == -1 ) ERROR_PRINT("Cannot find avatar %d in this game.\n", this->id);
			Avatar *nextAvatar = game->avatars[(index+1)%game->numAvatar];
			Avatar_equip(nextAvatar, game, bomb);
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
	int retIdx;
	while ( ( retIdx = Player_selectUse(this->player, game, this->cards, this->cards_size) ) != -1 ) {
		DEBUG_PRINT("Player %s want to use card \"%s\".\n", this->player->username, this->cards[retIdx]->name);
		DEBUG_PRINT("But he can't HAHAHAHA!\n");
	}
}

void Avatar_onDump(Avatar *this, Game *game) {
	if ( this->cards_size > this->hp ) {
		Player_chooseDrop(this->player, game, this->cards, this->cards_size, this->cards_size-this->hp);
	}
}

int Avatar_onReact(Avatar *this, Game *game, int card_id) {
	return 0;
}

void Avatar_dead(Avatar *this, Game *game) {
	int index = Game_find_index( game, this );
	if ( index == -1 ) ERROR_PRINT("Cannot find avatar %d in this game.\n", this->id);
	//discard cards
	for( int i = 0; i < this->cards_size ; i++ ) {
		Deck_put( game->discardPile, this->cards[i] );
	}
	//discard equipment
	if( this->equipment->armour != NULL) Deck_put( game->discardPile, this->equipment->armour );
	if( this->equipment->horseMinus != NULL) Deck_put( game->discardPile, this->equipment->horseMinus );
	if( this->equipment->horsePlus != NULL) Deck_put( game->discardPile, this->equipment->horsePlus );
	if( this->equipment->gun != NULL) Deck_put( game->discardPile, this->equipment->gun );
	if( this->equipment->bomb != NULL) Deck_put( game->discardPile, this->equipment->bomb );
	if( this->equipment->jail != NULL) Deck_put( game->discardPile, this->equipment->jail );
	//move dead people out
	for( int i = index ; i < game->numAvatar - 1; i++ ) {
		game->avatars[i] = game->avatars[i+1];
	}
	game->numAvatar --;
	Avatar_free(this);
	
}

void Avatar_hurt(Avatar *this, Game *game){
	this->hp -- ;
	if(this->hp == 0) {
		if( Avatar_onReact(this, game, CARD_BEER) == 0 ) {
			Avatar_dead(this, game);
		}else {
			this->hp ++ ;
		}
	}

	DEBUG_PRINT("Avatar %d hurt.\n", this->id);
	return;
}

void Avatar_heal(Avatar *this, Game *game){
	this->hp ++ ;
	DEBUG_PRINT("Avatar %d heal.\n", this->id);
	return;
}

void Avatar_equip(Avatar *this, Game *game, Card *card) {
	if( card->id > CARD_ARMOUR_START && card->id < CARD_ARMOUR_END ) {
		if( card->id == CARD_BARREL ) {
			this->equipment->armour = card;
		}else if ( card->id == CARD_SCOPE ) {
			this->equipment->horseMinus = card;
		}else if ( card->id == CARD_MUSTANG ) {
			this->equipment->horsePlus = card;
		}
	}else if ( card->id > CARD_GUN_START && card->id < CARD_GUN_END ) {
		if( this->equipment->gun != NULL){
			Card *trash = Avatar_unequip(this,game,&(this->equipment->gun));
			Deck_put(game->discardPile,trash);
		}else {
			this->equipment->gun = card;
		}
	}else if ( card->id > CARD_JUDGE_START && card->id < CARD_JUDGE_END ) {
		if( card->id == CARD_JAIL ) {
			this->equipment->jail = card;
		}else if ( card->id == CARD_DYNAMITE ) {
			this->equipment->bomb = card;
		}
	}
	DEBUG_PRINT("Avatar %d quipped the card: %s.\n", this->id , card->name);
	return;
}

Card* Avatar_unequip(Avatar *this, Game *game, Card **card){
	Card *bye = *card;
	*card = NULL;
	DEBUG_PRINT("Avatar %d unquipped the card: %s.\n", this->id , (*card)->name );
	return bye;
}

void Avatar_draw(Avatar *this, Game *game){
	this->cards_size ++;
	this->cards[this->cards_size - 1] = Deck_draw(game->deck);
	DEBUG_PRINT("Avatar %d draw one card.Remain: %d cards.\n", this->id ,game->deck->top + 1);
	return;
}

int* Avatar_choose(Avatar *this, Game *game, Card **options , int size, int num){	
	return NULL;
}

void Avatar_get(Avatar *this, Game *game, Card *want){
	this->cards_size ++;
	this->cards[this->cards_size - 1] = want;
	DEBUG_PRINT("Avatar %d get the card: %s.\n", this->id , want->name);
	return;
}

Card* Avatar_taken(Avatar *this, Game *game, int index){
	Card *bye = this->cards[index];
	for( int i = index ; i < this->cards_size - 1 ; i++ ){
		this->cards[index] = this->cards[index + 1];
	}
	this->cards_size -- ;
	DEBUG_PRINT("Avatar %d's card: %s had been taken.\n", this->id , bye->name );
	return bye;
}

