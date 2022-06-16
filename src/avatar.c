#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "avatar.h"
#include "debug.h"
#include "cardid.h"
#include "player.h"
#include "card.h"
#include "characterid.h"

Character *Character_init(int id, const char *name, const int hp, const char *intro) {
	if(name == NULL || intro == NULL)
		ERROR_PRINT("NULL pointer Mother Fucker !!\n");

	if(hp < 0 && hp > 4)
		ERROR_PRINT("hp must be between 0 and 4");

	Character *new = malloc(sizeof(Character));
	new->id = id;
	new->name = malloc(strlen(name) + 1);
	strcpy(new->name, name);
	new->hp = hp;
	new->intro = malloc(strlen(intro)+1);
	strcpy(new->intro, intro);
	return new;
} 
Character *Character_copy(Character *this) {
	Character *new = malloc(sizeof(Character));
	new->name = malloc(strlen(this->name) + 1);
	strcpy(new->name, this->name);
	new->hp = this->hp;
	new->intro = malloc(strlen(this->intro)+1);
	strcpy(new->intro, this->intro);
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

Equipment * Equipment_copy(Equipment *this) {
	Equipment * new = malloc(sizeof(Equipment));
	memset ( new, 0, sizeof(Equipment) );
	if ( this->gun ) new->gun = Card_copy(this->gun);
	if ( this->armour ) new->armour = Card_copy(this->armour);
	if ( this->horsePlus ) new->horsePlus = Card_copy(this->horsePlus);
	if ( this->horseMinus ) new->horseMinus = Card_copy(this->horseMinus);
	if ( this->jail ) new->jail = Card_copy(this->jail);
	if ( this->bomb ) new->bomb = Card_copy(this->bomb);
	return new;
}

void Equipment_free(Equipment *this) {
	if ( this->gun ) Card_free(this->gun);
	if ( this->armour ) Card_free(this->armour);
	if ( this->horsePlus ) Card_free(this->horsePlus);
	if ( this->horseMinus ) Card_free(this->horseMinus);
	if ( this->jail ) Card_free(this->jail);
	if ( this->bomb ) Card_free(this->bomb);
	//DEBUG_PRINT("Done Equipment_free\n");
}


Avatar *Avatar_init(int id, Character *character, Role role) {

	Avatar *new = malloc(sizeof(Avatar));

	new->id = id;
	new->player = Player_init(id); // NOTE: avatar->id == avatar->player->id
	new->isDead = false;

	new->character = Character_init(character->id, character->name, character->hp, character->intro);

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

	DEBUG_PRINT("Finish initiation of player %s with avatar %d\n", new->player->username, new->id);

	return new;
}

Avatar *Avatar_copy(Avatar *this) {
	Avatar *new = malloc(sizeof(Avatar));
	new->id = this->id;
	new->player = this->player; // Note: not deep copy
	new->isDead = this->isDead;
	new->character = Character_copy(this->character);
	new->hp_max = this->hp_max;
	new->hp = this->hp;
	new->role = this->role;
	new->cards_size = this->cards_size;
	new->cards = malloc(this->cards_size * sizeof(Card *));
	for ( int i=0; i<this->cards_size; i++ ) {
		new->cards[i] = Card_copy(this->cards[i]);
	}
	new->equipment = Equipment_copy(this->equipment);
	return new;
}

void Avatar_free(Avatar *this) {
	Player_free(this->player);
	Character_free(this->character);
	Equipment_free(this->equipment);
	for ( int i=0; i<this->cards_size; i++ ) {
		Card_free(this->cards[i]);
	}
	free(this->cards);
	free(this);
	DEBUG_PRINT("Done Avatar_free!\n");
}

void Avatar_freeCopy(Avatar *this) {
	//Player_free(this->player); THIS IS WHERE DIFFERENT
	Character_free(this->character);
	Equipment_free(this->equipment);
	for ( int i=0; i<this->cards_size; i++ ) {
		Card_free(this->cards[i]);
	}
	free(this->cards);
	free(this);
	//DEBUG_PRINT("Done Avatar_freeCopy!\n");
}

void Avatar_onTurn(Avatar *this, Game *game)  {

	DEBUG_PRINT("Avatar %d's turn.\n", this->id);

	bool jailed = false;
	Avatar_onJudge(this, game, &jailed);
	if ( jailed ) return;

	if(this->isDead == true) return;
	
	Avatar_onDraw(this, game);

	if(this->isDead == true) return;

	Avatar_onPlay(this, game);

	if(this->isDead == true) return;
	
	Avatar_onDump(this, game);
}
	

void Avatar_onJudge(Avatar *this, Game *game, bool *jailed) {

	// TODO: Character ability - Lucky Duke

	if ( this->equipment->bomb != NULL ) {
	
		DEBUG_PRINT("Avatar %d judge for bomb.\n", this->id);
		Card *bomb = Avatar_unequip(this, game, &(this->equipment->bomb));
		if ( Avatar_judge(this, game, CARD_DYNAMITE) == 0) {
			// suit is between [Spade 2, Spade 9]
			Deck_put(game->discardPile, bomb);
			for ( int _=0; _<3; _++ ) Avatar_hurt(this, game, NULL);

		} else {
			// Find next avatar
			Avatar *nextAvatar = Game_nextAvailableAvatar(this);
			Avatar_equip(nextAvatar, game, bomb);
		}
		
	}
	if ( this->equipment->jail != NULL ) {

		DEBUG_PRINT("Avatar %d judge for Jail.\n", this->id);

		if (Avatar_judge(this,game,CARD_JAIL) == -1) {
			// suit is not heart
			*jailed = true;
		}
		Card *jail = Avatar_unequip(this, game, &(this->equipment->jail));
		Deck_put(game->discardPile, jail);
	}
	return;
}

void Avatar_onDraw(Avatar *this, Game *game) {
	// TODO: Character ability - Black Jack
	// TODO: Character ability - Jesse Jones
	// TODO: Character ability - Kit Carlson
	// TODO: Character ability - Pedro Ramirez
	if(this->character->id == Black_Jack) {
		Avatar_draw(this, game);
		Card* card = calloc(1,sizeof(Card));
		
		card = Deck_draw(game->deck);
		Avatar_get(this,game,card);
		DEBUG_PRINT("%s using his ability, the card he draw is %s,and the suit is %d,",this->player->username,card->name,card->suit);
		if ( card->suit >= 13 && card->suit <= 38 ) {
			DEBUG_PRINT("he can draw onemore card!\n");
			Avatar_draw(this,game);
		}else {
			DEBUG_PRINT("so sad, nope!\n");
		}
	}
	else if( this->character->id == Jesse_Jones) {
		Player* targetp = calloc(1,sizeof(Player));
		Avatar* target = Game_nextAvailableAvatar(this);
		while(1) {
			if( Player_useAbility(this->player,game) == true) {
				while(1) {
					targetp = Player_selectTarget(this->player,game);
					if(targetp->id == this->id) {
						WARNING_PRINT("You can't choose yourself!\n");
						continue;
					}else {
						break;
					}
				}
			do {
					if(targetp->id == target->id) {
						break;
					}
					target = Game_nextAvailableAvatar(target);
				} while ( target->id != this->id);
				if(target->cards_size == 0 ) {
					WARNING_PRINT("The target you choose have no cards left!\n");
					continue;
				}else {
					int *choose = Avatar_choose(this,game,target->cards,target->cards_size,1);
					Avatar_get(this,game,Avatar_taken(target, game, choose[0]));
					break;
				}
			}else {
				Avatar_draw(this,game);
				break;
			}
		}
		Avatar_draw(this,game);
	}
	else if( this->character->id == Pedro_Ramirez && game->discardPile->top >= 0) {
		if( Player_useAbility(this->player,game) == true) {
			Avatar_get(this,game,Deck_draw(game->discardPile));
		}else {
			Avatar_draw(this,game);
		}
		Avatar_draw(this,game);
	}
	else if( this->character->id == Kit_Carlson) {
		Card **options = calloc(3,sizeof(Card));
		int* choosen = calloc(2,sizeof(int));
		for(int i = 0; i<3 ; i++) {
			options[i] = Deck_draw(game->deck);
		}
		choosen = Avatar_choose(this,game,options,3,2);
		for(int i = 0; i<3 ; i++) {
			if ( i != choosen[0] && i != choosen[1]) {
				Deck_put(game->deck,options[i]);
			}else {
				Avatar_get(this,game,options[i]);
			}
		}
	}
	else {
		Avatar_draw(this,game);
		Avatar_draw(this,game);
	}
}
	

void Avatar_onPlay(Avatar *this, Game *game) {
	int retIdx;
	bool banged = false;
	while ( ( retIdx = Player_selectUse(this->player, game, this->cards, this->cards_size) ) != -1 ) {

		DEBUG_PRINT("Player %s want to use card \"%s\".\n", this->player->username, this->cards[retIdx]->name);

		Card *card = this->cards[retIdx];
		if ( this->character->id == Calamity_Janet && card->id == CARD_MISS ) {
			card -> type = CARD_DIST_VISION;
		}

		Player *tarPlayer = NULL;
		if ( card->type != CARD_DIST_NON ) {
			tarPlayer = Player_selectTarget(this->player, game);
		}

		Avatar *tar = tarPlayer ? game->avatars[tarPlayer->id-1] : NULL;

		bool valid = validPlay(this, tar, card);

		if ( valid && banged && card->play == &play_CARD_BANG ) {
			// TODO: Character ability - Willy the Kid
			// TODO: VOLCANIC
			if ( ( this->equipment->gun == NULL || this->equipment->gun->id != CARD_VOLCANIC ) && this->character->id != Willy_the_Kid ) {
				WARNING_PRINT("You cannot use BANG! twice.\n");
				valid = false;
			}
		}

		DEBUG_PRINT("Finish checking validation: %s\n", valid?"Valid":"Invalid");

		if ( valid ) {

			card = Avatar_taken(this, game, retIdx);
			if ( CARD_HAND_START < card->id && card->id < CARD_HAND_END ) {
				Deck_put(game->discardPile, card);
			}

			if ( card->id == CARD_BANG ) {
				banged = true;
			}
			if ( this->character->id == Calamity_Janet && card->id == CARD_MISS ) {
				card->type = CARD_DIST_NON;
				play_CARD_BANG(this, tar, game, card);
				banged = true;
			} else {
				card->play(this, tar, game, card);
			}
		}
	}
	DEBUG_PRINT("Player %s's turn end.\n", this->player->username);
}

void Avatar_onDump(Avatar *this, Game *game) {
	if ( this->cards_size > this->hp ) {
		int n = this->cards_size - this->hp;
		int *indexes = Player_chooseDrop(this->player, game, this->cards, this->cards_size, n);
		for ( int i=n-1; i>=0; i-- ) {
			Deck_put( game->discardPile, Avatar_taken(this, game, indexes[i]) );
		}
		free(indexes);
	}
}

int Avatar_onReact(Avatar *this, Game *game, int card_id, Card* to_react) {
	// fin TODO: Character ability - Jourdonnais
	// fin TODO: Equipment - Barrel 
	// TODO: Character ability - Sid Ketchum
	DEBUG_PRINT( "Ask Avatar %d to respond for %s Remain: %d\n", this->id, to_react ? to_react->name : "(NULL)" ,this->cards_size);
	if( (this->equipment->armour != NULL || this->character->id == Jourdonnais) && to_react && to_react->id == CARD_BANG) {
		if( Avatar_judge(this,game,CARD_BARREL) == 0)return 0;
	}
	while(1) {
		int react = Player_selectReact(this->player, game, this->cards, this->cards_size);
		if ( react == -1) {
			DEBUG_PRINT("Finish react 1\n");
			return -1;
		} else {
			Card *reactCard = this->cards[react];
			if ( reactCard == NULL ) ERROR_PRINT("ereere.\n");
			if ( reactCard->id == card_id ) {
				Deck_put(game->discardPile, Avatar_taken(this, game, react));
				DEBUG_PRINT("Finish react 2\n");
				return 0;
			} else if ( this->character->id == Calamity_Janet && 
				 	( ( reactCard->id == CARD_BANG && card_id == CARD_MISS ) ||
					( reactCard->id == CARD_MISS && card_id == CARD_BANG ) ) ) {
				Deck_put(game->discardPile, Avatar_taken(this, game, react));
				DEBUG_PRINT("Finish react 3\n");
				return 0;
			} else {
				WARNING_PRINT("You can't react with this card !\n");
				continue;
			}
		}
	}
}

int Avatar_judge(Avatar *this, Game *game, int card_id) {
	Card *card = calloc(1,sizeof(Card));
	if(this->character->id == Lucky_Duke) {
		Card **options = calloc(2,sizeof(Card));
		for(int i = 0; i<2; i++) {
			options[i] = Deck_draw(game->deck);
		}
		int*choosen = Avatar_choose(this,game,options,2,1);
		for(int i = 0; i<2 ; i++) {
			if ( i != choosen[0]) {
				Deck_put(game->discardPile,options[i]);
			}else {
				card = options[i];
				Deck_put(game->discardPile,options[i]);
			}
		}
	}else {
		card = Deck_draw(game->deck);
		Deck_put(game->discardPile, card);
	}
	if( card_id == CARD_DYNAMITE) {
		if ( card->suit >= 1 && card->suit <= 8 ) {
			return 0;
		}else {
			return -1;
		}
	}
	if( card_id == CARD_JAIL || card_id == CARD_BARREL ) { 
		if ( card->suit >= 13 && card->suit <= 25 ) {
			return 0;
		}else {
			return -1;
		}
	}
	return 0;
}
void Avatar_dead(Avatar *this, Game *game) {
	// TODO: Character ability - Vulture Sam
	//discard cards
	DEBUG_PRINT("Avatar %d dead.\n", this->id);
	Avatar* next = Game_nextAvailableAvatar(this);;
	int check = -1;
	do {
		if(next->character->id == Vulture_Sam) {
			check = 0;
			break;
		}
		next = Game_nextAvailableAvatar(next);
	}while(next->character->id != this->character->id) ;
	
	if( check == 0 ) {
		for( int i = this->cards_size -1; i >= 0 ; i-- ) {
			Avatar_get(next,game,Avatar_taken(this, game, i));
		}
	}else {
		for( int i = this->cards_size -1; i >= 0 ; i-- ) {
			Deck_put( game->discardPile, this->cards[i] );
			Avatar_taken(this, game, i);
		}
		this->cards_size = 0;
	}
	//discard equipment
	if( this->equipment->armour != NULL) {
		if( check == 0) {
			Avatar_get(next,game,Avatar_unequip(this,game,&(this->equipment->armour)));
		}
		else {
			Deck_put( game->discardPile,Avatar_unequip(this,game,&(this->equipment->armour)));
		}
		
	}
	if( this->equipment->horseMinus != NULL) {
		if( check == 0) {
			Avatar_get(next,game,Avatar_unequip(this,game,&(this->equipment->horseMinus)));
		}
		else {
			Deck_put( game->discardPile,Avatar_unequip(this,game,&(this->equipment->horseMinus)));
		}
		
	}
	if( this->equipment->horsePlus != NULL) {
		if( check == 0) {
			Avatar_get(next,game,Avatar_unequip(this,game,&(this->equipment->horsePlus)));
		}
		else {
			Deck_put( game->discardPile,Avatar_unequip(this,game,&(this->equipment->horsePlus)));
		}
		
	}
	if( this->equipment->gun != NULL) {
		if( check == 0) {
			Avatar_get(next,game,Avatar_unequip(this,game,&(this->equipment->gun)));
		}
		else {
			Deck_put( game->discardPile,Avatar_unequip(this,game,&(this->equipment->gun)));
		}
		
	}
	if( this->equipment->bomb != NULL) {
		if( check == 0) {
			Avatar_get(next,game,Avatar_unequip(this,game,&(this->equipment->bomb)));
		}
		else {
			Deck_put( game->discardPile,Avatar_unequip(this,game,&(this->equipment->bomb)) );
		}
		
	}
	if( this->equipment->jail != NULL) {
		if( check == 0) {
			Avatar_get(next,game,Avatar_unequip(this,game,&(this->equipment->jail)));
		}
		else {
			Deck_put( game->discardPile, Avatar_unequip(this,game,&(this->equipment->jail)) );
		}
		
	}
	// TODO: Show role card
	//move dead people out
	this->isDead = true;
	game->numAvailableAvatar--;
	Game_checkWin(game);
	printf("%s is dead\n",this->player->username);
}

void Avatar_hurt(Avatar *this, Game *game, Avatar *attacker){
	// TODO: Character ability - Bart Cassidy 
	// TODO: Character ability - El Gringoy
	if(this->isDead == true) return;
	this->hp -- ;
	printf("%s's hp -1 Remain: %d\n",this->player->username,this->hp);
	if(this->hp == 0) {
		printf("Oh no %s's hp equal 0,",this->player->username);
		if( Avatar_onReact(this, game, CARD_BEER, NULL) == -1 || game->numAvailableAvatar <= 2) {
			Avatar_dead(this, game);
			return;
		}else {
			this->hp ++ ;
			printf("but he use beer to heal himself\n");
		}
	}
	if( this->character->id == Bart_Cassidy) {
		DEBUG_PRINT("%s hurt, using his ability.\n",this->player->username);
		Avatar_draw(this,game);
	}
	if( attacker && this->character->id == El_Gringo && attacker->cards_size != 0) {
		DEBUG_PRINT("%s hurt, using his ability.\n",this->player->username);
		int *choose = Avatar_choose(this,game,attacker->cards,attacker->cards_size,1);
		Avatar_get(this,game,Avatar_taken(attacker,game,choose[0]));
	}


	DEBUG_PRINT("Avatar %d hurt.\n", this->id);
	return;
}

void Avatar_heal(Avatar *this, Game *game){
	this->hp ++;
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
	}else if( card->id == CARD_JAIL ) {
		this->equipment->jail = card;
	}else if ( card->id == CARD_DYNAMITE ) {
		if( this->equipment->bomb != NULL) {
			Card *trash = Avatar_unequip(this,game,&(this->equipment->bomb));
			Deck_put(game->discardPile,trash);
			WARNING_PRINT("Because %s equipped the same equipment,the previous card had been discard!\n",this->player->username);
		}
		this->equipment->bomb = card;
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
	Avatar_get(this,game, Deck_draw(game->deck));
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
	if( this->character->id == Suzy_Lafayette && this->cards_size == 0 && this->isDead == false) {
		Avatar_draw(this,game);
		printf("%s have no card! Using his ability.\n",this->player->username);
	}
	DEBUG_PRINT("Avatar %d's card: %s had been taken.\n", this->id , bye->name );
	return bye;
}

int Avatar_calcDist(Game *game, Avatar *this, Avatar *that) {
	int idx_1 = Game_findIndex(this);
	int idx_2 = Game_findIndex(that);

	int dist = abs(idx_2 - idx_1);
	if(dist > (double)game->numAvailableAvatar / 2) {
		dist = -1 * dist + game->numAvailableAvatar; 
	}

	if ( this->character->id == Rose_Doolan ) {
		dist--;
	}
	if ( this->equipment->horseMinus ) {
		dist--;
	}
	if ( that->character->id == Paul_Regret ) {
		dist++;
	}
	if ( that->equipment->horsePlus ) {
		dist++;
	}

	return dist;
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
	
	DEBUG_PRINT("Calc vision for avatar %d Done.\n", this->id);
	return dist;
}

Card **Avatar_giveToChoose(Avatar *this, int *retSize) {
	int counter = 0 ;
	for ( Card ** iter = (Card **)this->equipment; iter < (Card **)(this->equipment+1); iter++) {
		if ( *iter ) counter++;
	}
	*retSize = this->cards_size+counter;

	Card **list = malloc( (*retSize) * sizeof(Card *) );
	counter = 0;
	memset(list, 0, this->cards_size * sizeof(Card *));
	for ( Card ** iter = (Card **)this->equipment; iter < (Card **)(this->equipment+1); iter++) {
		if ( *iter ) list[this->cards_size+counter] = *iter;
	}
	return list;
}