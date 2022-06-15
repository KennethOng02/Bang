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


Avatar *Avatar_init(int id, Character *character, Role role) {

	Avatar *new = malloc(sizeof(Avatar));

	new->id = id;
	new->isDead = false;

	new->character = Character_init(character->name, character->hp, character->intro);

	new->hp_max = new->character->hp;
	new->role = role;
	if(new->role == SHERIFF) {
		DEBUG_PRINT("SHERIFF is avatar %d.\n", id);
		new->hp_max++;
	}
	new->hp = new->hp_max;
	new->cards_size = 0;
	new->cards = malloc(CARDS_SIZE_MAX * sizeof(Card *));

	new->equipment = Equipment_init();


	return new;
}

void Avatar_free(Avatar *this) {
	// NOTE: Players does't free here
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

	// TODO: Character ability - Lucky Duke

	if ( this->equipment->bomb != NULL ) {
	
		DEBUG_PRINT("Avatar %d judge for bomb.\n", this->id);

		Card *card = Deck_draw(game->deck);
		int suit = card->suit;
		Deck_put(game->discardPile, card);

		Card *bomb = Avatar_unequip(this, game, &(this->equipment->bomb));
		if ( suit >= 1 && suit <= 8 ) {
			// suit is between [Spade 2, Spade 9]

			Deck_put(game->discardPile, bomb);
			for ( int _=0; _<3; _++ ) Avatar_hurt(this, game, NULL);

		} else {
			// Find next avatar
			Avatar *nextAvatar = Game_nextAvailableAvatar(game, this);
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
	// TODO: Character ability - Black Jack
	// TODO: Character ability - Jesse Jones
	// TODO: Character ability - Kit Carlson
	// TODO: Character ability - Pedro Ramirez 
	for ( int _=0; _<2; _++ ) Avatar_draw(this, game);
}

void Avatar_onPlay(Avatar *this, Game *game) {
	// TODO: implimentation
	int retIdx;
	Player *tarPlayer;
	bool banged = false;
	while ( ( retIdx = Player_selectUse(this->player, game, this->cards, this->cards_size, &tarPlayer) ) != -1 ) {
		DEBUG_PRINT("Player %s want to use card \"%s\".\n", this->player->username, this->cards[retIdx]->name);

		bool valid = true;
		Avatar *tar = tarPlayer ? tarPlayer->avatar : NULL;

		Card *card = this->cards[retIdx];

		for( int i = retIdx ; i < this->cards_size - 1 ; i++ ){
			this->cards[i] = this->cards[i + 1];
		}
		this->cards_size-- ;

		if ( tar && tar->isDead ) {
			WARNING_PRINT("Cannot play %s on dead player.\n", card->name);
			valid = false;
		}
		if ( card->type != CARD_DIST_NON ) {
			if ( tar == NULL ) {
				WARNING_PRINT("No target when playing %s.\n", card->name);
				valid = false;
			} else if ( tar->id == this->id ) {
				WARNING_PRINT("Cannot play %s on self.\n", card->name);
				valid = false;
			} else if ( card->type == CARD_DIST_ONE && Avatar_calcDist(game, this, tar) > 1 ) {
				WARNING_PRINT("Cannot play %s on %s: Too far.\n", card->name, tar->player->username);
				valid = false;
			} else if ( card->type == CARD_DIST_VISION && Avatar_calcDist(game, this, tar) > Avatar_calcVision(this) ) {
				WARNING_PRINT("Cannot play %s on %s: Too far.\n", card->name, tar->player->username);
				valid = false;
			}
		}
		if ( banged && card->play == &play_CARD_BANG ) {
			// TODO: Character ability - Willy the Kid
			// TODO: VOLCANIC
			WARNING_PRINT("You cannot use BANG! twice.\n");
			valid = false;
		}
		
		if ( valid ) {
			if ( card->play(this, tar, game, card) == -1 ) {
				WARNING_PRINT("Cannot play %s here\n", card->name);
				valid = false;
			}
		}

		if ( valid ) {
			if ( card->play == &play_CARD_BANG ) {
				banged = true;
			}
			Deck_put(game->discardPile, card);
		} else {
			for( int i = this->cards_size-1 ; i >= retIdx ; i-- ){
				this->cards[i+1] = this->cards[i];
			}
			this->cards[retIdx] = card;
			this->cards_size++;
		}
	}
	DEBUG_PRINT("Player %s's turn end.\n", this->player->username);
}

void Avatar_onDump(Avatar *this, Game *game) {
	if ( this->cards_size > this->hp ) {
		int n = this->cards_size - this->hp;
		int *indexes = Player_chooseDrop(this->player, game, this->cards, this->cards_size, n);
		for ( int i=0; i<n; i++ ) {
			Deck_put( game->discardPile, Avatar_taken(this, game, indexes[i]) );
		}
		free(indexes);
	}
}

int Avatar_onReact(Avatar *this, Game *game, int card_id) {
	// TODO: Character ability - Calamity Janet
	// TODO: Character ability - Jourdonnais
	// TODO: Equipment - Barrel
	// TODO: Character ability - Sid Ketchum
	while(1) {
		int react = Player_selectReact(this->player, game, this->cards, this->cards_size);
		if ( react == -1) {
			return -1;
		}else {
			if( this->cards[react]->id == card_id ) {
				return 0;
			}else {
				WARNING_PRINT("You can't react with this card !");
				continue;
			}
		}
	}
}

void Avatar_dead(Avatar *this, Game *game) {
	// TODO: Character ability - Vulture Sam
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
	// TODO: Show role card
	//move dead people out
	this->isDead = true;
	game->numAvailablePlayer--;
	Game_checkWin(game);
	printf("%s is dead\n",this->player->username);
}

void Avatar_hurt(Avatar *this, Game *game, Avatar *attacker){
	// TODO: Character ability - Bart Cassidy 
	// TODO: Character ability - El Gringoy
	this->hp -- ;
	printf("%s's hp -1\n",this->player->username);
	if(this->hp == 0) {
		printf("Oh no %s's hp equal 0,",this->player->username);
		if( Avatar_onReact(this, game, CARD_BEER) == -1 || game->numAvailablePlayer <= 2) {
			Avatar_dead(this, game);
		}else {
			this->hp ++ ;
			printf("but he use beer to heal himself\n");
		}
	}

	DEBUG_PRINT("Avatar %d hurt.\n", this->id);
	return;
}

void Avatar_heal(Avatar *this, Game *game){
	this->hp ++ ;
	printf("%s's hp +1\n",this->player->username);
	DEBUG_PRINT("Avatar %d heal.\n", this->id);
	return;
}

void Avatar_equip(Avatar *this, Game *game, Card *card) {
	if( card->id > CARD_ARMOUR_START && card->id < CARD_ARMOUR_END ) {
		if( card->id == CARD_BARREL ) {
			if( this->equipment->armour != NULL) {
				Card *trash = Avatar_unequip(this,game,&(this->equipment->armour));
				Deck_put(game->discardPile,trash);
				WARNING_PRINT("Because %s equipped the same equipment,the previous card had been discard!\n",this->player->username);
			}
			this->equipment->armour = card;
		}else if ( card->id == CARD_SCOPE ) {
			if( this->equipment->horseMinus != NULL) {
				Card *trash = Avatar_unequip(this,game,&(this->equipment->horseMinus));
				Deck_put(game->discardPile,trash);
				WARNING_PRINT("Because %s equipped the same equipment,the previous card had been discard!\n",this->player->username);
			}
			this->equipment->horseMinus = card;
		}else if ( card->id == CARD_MUSTANG ) {
			if( this->equipment->horsePlus != NULL) {
				Card *trash = Avatar_unequip(this,game,&(this->equipment->horsePlus));
				Deck_put(game->discardPile,trash);
				WARNING_PRINT("Because %s equipped the same equipment,the previous card had been discard!\n",this->player->username);
			}
			this->equipment->horsePlus = card;
		}
	}else if ( card->id > CARD_GUN_START && card->id < CARD_GUN_END ) {
		if( this->equipment->gun != NULL) {
			Card *trash = Avatar_unequip(this,game,&(this->equipment->gun));
			Deck_put(game->discardPile,trash);
			WARNING_PRINT("Because %s can only have one gun,the previous card had been discard!\n",this->player->username);
		}
		this->equipment->gun = card;
	}else if ( card->id > CARD_JUDGE_START && card->id < CARD_JUDGE_END ) {
		if( card->id == CARD_JAIL ) {
			this->equipment->jail = card;
		}else if ( card->id == CARD_DYNAMITE ) {
			if( this->equipment->bomb != NULL) {
				Card *trash = Avatar_unequip(this,game,&(this->equipment->bomb));
				Deck_put(game->discardPile,trash);
				WARNING_PRINT("Because %s equipped the same equipment,the previous card had been discard!\n",this->player->username);
			}
		this->equipment->bomb = card;
		}
	}
	DEBUG_PRINT("Avatar %d quipped the card: %s.\n", this->id , card->name);
	return;
}

Card* Avatar_unequip(Avatar *this, Game *game, Card **card){
	Card *bye = *card;
	DEBUG_PRINT("Avatar %d unquipped the card: %s.\n", this->id , (*card)->name );
	*card = NULL;
	return bye;
}

void Avatar_draw(Avatar *this, Game *game){
	this->cards_size ++;
	this->cards[this->cards_size - 1] = Deck_draw(game->deck);
	DEBUG_PRINT("Avatar %d draw one card.Remain: %d cards.\n", this->id ,game->deck->top + 1);
	return;
}

int* Avatar_choose(Avatar *this, Game *game, Card **options , int size, int num) {
	return Player_chooseTake( this->player, game, options, size, num );
}

void Avatar_get(Avatar *this, Game *game, Card *want){
	this->cards_size ++;
	this->cards[this->cards_size - 1] = want;
	DEBUG_PRINT("Avatar %d get the card: %s.\n", this->id , want->name);
	return;
}

Card* Avatar_taken(Avatar *this, Game *game, int index){
	// TODO: Character ability - Suzy Lafayette
	Card *bye = this->cards[index];
	for( int i = index ; i < this->cards_size - 1 ; i++ ){
		this->cards[i] = this->cards[i+1];
	}
	this->cards_size -- ;
	DEBUG_PRINT("Avatar %d's card: %s had been taken.\n", this->id , bye->name );
	return bye;
}

int Avatar_calcDist(Game *game, Avatar *this, Avatar *that) {
	int idx_1 = Game_findIndex(game, this);
	int idx_2 = Game_findIndex(game, that);

	int dist = abs(idx_2 - idx_1);
	if(dist <= game->numAvailablePlayer / 2) {
		return dist;
	}else {
		return -1 * dist + game->numAvailablePlayer; 
	}
}

int Avatar_calcVision(Avatar *this) {
	int dist = 1;
	DEBUG_PRINT("Calc vision for avatar %d\n", this->id);

	if ( this->equipment->gun ) {
		Card *gun = this->equipment->gun;
		if(gun->id == CARD_VOLCANIC)
			dist = 1;
		if(gun->id == CARD_SCHOFIELD)
			dist = 2;
		if(gun->id == CARD_REMINGTON)
			dist = 3;
		if(gun->id == CARD_CARABINE)
			dist = 4;
		if(gun->id == CARD_WINCHEDTER)
			dist = 5;
	}
	return dist;
}
