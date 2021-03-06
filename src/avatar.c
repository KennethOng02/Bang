#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include <unistd.h>

#include "avatar.h"
#include "print.h"
#include "cardid.h"
#include "player.h"
#include "card.h"
#include "characterid.h"
#include "interface.h"
#include "mylib.h"

Character *Character_init(int id, const char *name, const int hp, const char *intro) {
	if(name == NULL || intro == NULL)
		ERROR_PRINT("NULL pointer Mother Fucker !!");

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
	DEBUG_PRINT("Done Equipment_free\n");
}


Avatar *Avatar_init(int id, Character *character, Role role) {

	Avatar *new = malloc(sizeof(Avatar));

	new->id = id;
	new->player = Player_init(id, new); // NOTE: avatar->id == avatar->player->id
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
}

void Avatar_onTurn(Avatar *this, Game *game)  {

	MESSAGE_PRINT("%s's turn.", this->player->username);

	bool jailed = false;
	Avatar_onJudge(this, game, &jailed);
	if ( jailed ) return;

	if(this->isDead == true) return;
	Avatar_onDraw(this, game);
	if(this->isDead == true) return;

	Avatar_onPlay(this, game);

	if(this->isDead == true) return;
	
	Avatar_onDump(this, game);

	MESSAGE_PRINT("%s's turn finish.", this->player->username);
}
	

void Avatar_onJudge(Avatar *this, Game *game, bool *jailed) {

	// TODO: Character ability - Lucky Duke

	if ( this->equipment->bomb != NULL ) {
	
		MESSAGE_PRINT("%s judge for dynamite.", this->player->username);
		Card *bomb = Avatar_unequip(this, game, &(this->equipment->bomb));
		if ( Avatar_judge(this, game, CARD_DYNAMITE) == 0) {
			// suit is between [Spade 2, Spade 9]
			Avatar_discard(game, bomb);
			for ( int _=0; _<3; _++ ) Avatar_hurt(this, game, NULL);

		} else {
			// Find next avatar
			Avatar *nextAvatar = Game_nextAvailableAvatar(this);
			Avatar_equip(nextAvatar, game, bomb);
		}
		
	}
	if ( this->equipment->jail != NULL ) {

		MESSAGE_PRINT("%s judge for Jail.", this->player->username);

		if (Avatar_judge(this,game,CARD_JAIL) == -1) {
			// suit is not heart
			*jailed = true;
		}
		Card *jail = Avatar_unequip(this, game, &(this->equipment->jail));
		Avatar_discard(game, jail);
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
		MESSAGE_PRINT("%s using his ability(%s), the card he draw is %s,and the suit is %s.",this->player->username,this->character->name,card->name,interface_getCardSuit(card->suit));
		if ( card->suit >= 13 && card->suit <= 38 ) {
			MESSAGE_PRINT("he can draw one more card!");
			Avatar_draw(this,game);
		}else {
			MESSAGE_PRINT("so sad, he can't draw one more card!");
		}
	}
	else if( this->character->id == Jesse_Jones) {
		bool *validTargets = malloc(game->numAvatar * sizeof(bool));
		bool validTargets_any = false;
		for ( int i=0; i<game->numAvatar; i++ ) {
			validTargets[i] = game->avatars[i]->id != this->id && !game->avatars[i]->isDead && game->avatars[i]->cards_size > 0;
			if ( validTargets[i] ) validTargets_any = true;
		}
		bool drawed = false;
		if( validTargets_any ) {
			while ( Player_useAbility(this->player, game) ) {
				int retIdx = -1;
				while(1) {
					retIdx = Player_selectTarget(this->player,game, validTargets);
					if ( retIdx < -1 || retIdx >= game->numAvatar ) ERROR_PRINT("Invalid index.");
					if ( retIdx == -1 || validTargets[retIdx] ) break;
				}
				if ( retIdx == -1 ) continue;
				drawed = true;
				Avatar *target = game->avatars[retIdx];
				// Play
				Card **list = malloc( target->cards_size * sizeof(Card *) );
				memset(list, 0, target->cards_size * sizeof(Card *));
				int *choose = Avatar_choose(this,game,list,target->cards_size,1, false);
				MESSAGE_PRINT("%s using his ability(%s),he draw %s's card",this->player->username,this->character->name,target->player->username);
				Avatar_get(this,game,Avatar_taken(target, game, choose[0]));
				free(list);
				break;
			}
		}
		if ( !drawed ) Avatar_draw(this,game);
		Avatar_draw(this,game);
		free(validTargets);
	}
	else if( this->character->id == Pedro_Ramirez && game->discardPile->top >= 0) {
		if( Player_useAbility(this->player,game) == true) {
			MESSAGE_PRINT("%s using his ability(%s)",this->player->username,this->character->name);
			Avatar_get(this,game,Deck_draw(game->discardPile));
		}else {
			Avatar_draw(this,game);
		}
		Avatar_draw(this,game);
	}
	else if( this->character->id == Kit_Carlson) {
		Card **options = calloc(3,sizeof(Card));
		int* choosen ;
		MESSAGE_PRINT("%s using his ability(%s)",this->player->username,this->character->name);
		for(int i = 0; i<3 ; i++) {
			options[i] = Deck_draw(game->deck);
		}
		choosen = Avatar_choose(this,game,options,3,2, false);
		for(int i = 0; i<3 ; i++) {
			if ( i != choosen[0] && i != choosen[1]) {
				Avatar_discard(game,options[i]);
			} else {
				Avatar_get(this,game,options[i]);
			}
		}
		free(options);
	}
	else {
		Avatar_draw(this,game);
		Avatar_draw(this,game);
	}
}
	

void Avatar_onPlay(Avatar *this, Game *game) {
	bool banged = false;
	while ( 1 ) {

		int orgCards_size = this->cards_size;

		// validPlays[i][j]: whether cards[i] can be played on avatars[j]
		bool **validPlays = malloc(this->cards_size * sizeof(bool *));
		for ( int i=0; i<this->cards_size; i++ ) {
			validPlays[i] = malloc(game->numAvatar * sizeof(bool));
		}
		bool repeater = ( this->equipment->gun && this->equipment->gun->id == CARD_VOLCANIC ) || this->character->id == Willy_the_Kid;
		for ( int i=0; i<this->cards_size; i++ ) {
			Card *card = this->cards[i];
			bool isBang = card->id == CARD_BANG || (this->character->id == Calamity_Janet && card->id == CARD_MISS);
			for ( int j=0; j<game->numAvatar; j++ ) {
				validPlays[i][j] = validPlay(this, game->avatars[j], this->cards[i]);
				if ( isBang && banged && !repeater ) {
					validPlays[i][j] = false;
				}
			}
		}

		// validPlays_any[i]: whether cards[i] can be played
		bool *validPlays_any = malloc(this->cards_size * sizeof(bool));
		// valid_any: whether there is a card can be played
		bool valid_any = false;
		for ( int i=0; i<this->cards_size; i++ ) {
			validPlays_any[i] = false;
			for ( int j=0; j<game->numAvatar; j++ ) {
				if ( validPlays[i][j] ) {
					validPlays_any[i] = true;
					break;
				}
			}
			if ( validPlays_any[i] ) valid_any = true;
		}
		DEBUG_PRINT("Finish cheking whether card can be play.\n");


		// Ask for what he want to play
		int retIdx;
		bool canUseAbility = false;
		if ( this->character->id == Sid_Ketchum && this->hp < this->hp_max && this->cards_size >= 2 ) canUseAbility = true;
		while ( 1 ) {
			retIdx = Player_selectUse(this->player, game, this->cards, validPlays_any, this->cards_size, canUseAbility);
			if ( retIdx >= this->cards_size || retIdx < -2 ) ERROR_PRINT("Invalid index.");
			if ( retIdx == -1 || validPlays_any[retIdx] ) break;
			if ( canUseAbility && retIdx == -2 ) break; // Use ability
		}
		if ( retIdx == -1 ) {
			DEBUG_PRINT("%s finish his turn.\n", this->player->username);
			for ( int i=0; i<orgCards_size; i++ ) free(validPlays[i]);
			free(validPlays_any);
			free(validPlays);
			return;
		}
		if ( canUseAbility && retIdx == -2 ) {
			if ( this->character->id == Sid_Ketchum ) {
				int *rets = Player_chooseDrop(this->player, game, this->cards, this->cards_size, 2, true);
				if ( rets != NULL ) {
					Card* trash;
					MESSAGE_PRINT("%s use his ability(%s).",this->player->username,this->character->name);
					for ( int i=1; i>=0; i-- ) { // Don't change this!
						trash = Avatar_taken(this, game, rets[i]);
						Avatar_discard(game, trash);
						MESSAGE_PRINT("%s discard the card %s",this->player->username,trash->name);
					}
					Avatar_heal(this, game);
					free(rets);
				}
				for ( int i=0; i<orgCards_size; i++ ) free(validPlays[i]);
				free(validPlays_any);
				free(validPlays);
				continue;
			}
		}
		DEBUG_PRINT("Finish asking what card he want to play.\n");

		// Ask for target
		Card *card = this->cards[retIdx];
		bool calamity_bang = this->character->id == Calamity_Janet && card->id == CARD_MISS;
		int retPlayerIdx = -1;
		if ( card->type != CARD_DIST_NON || calamity_bang ) {
			while ( 1 ) {
				retPlayerIdx = Player_selectTarget(this->player, game, validPlays[retIdx]);
				if ( retPlayerIdx >= game->numAvatar || retPlayerIdx < -1 ) ERROR_PRINT("Invalid index."); 
				if ( retPlayerIdx == -1 || validPlays[retIdx][retPlayerIdx] ) break;
			}
			if ( retPlayerIdx == -1 ) continue;
		}

		// Play card
		Avatar *tarAvatar = retPlayerIdx == -1 ? NULL : game->avatars[retPlayerIdx];
		card = Avatar_taken(this, game, retIdx);
		if ( tarAvatar != NULL ) {
			DEBUG_PRINT("%s use card \"%s\" on %s.\n", this->player->username, card->name, tarAvatar->player->username);
		} else {
			DEBUG_PRINT("%s use card \"%s\".\n", this->player->username, card->name);
		}
		if ( CARD_HAND_START < card->id && card->id < CARD_HAND_END ) {
			Avatar_discard(game, card);
		}
		if ( card->id == CARD_BANG || calamity_bang ) {
			play_CARD_BANG(this, tarAvatar, game, card);
			banged = true;
		} else {
			card->play(this, tarAvatar, game, card);
		}
		
		// free
		for ( int i=0; i<orgCards_size; i++ ) {
			free(validPlays[i]);
		}
		free(validPlays_any);
		free(validPlays);
	}
	DEBUG_PRINT("%s's turn end.\n", this->player->username);
}

void Avatar_onDump(Avatar *this, Game *game) {
	Card* trash;
	if ( this->cards_size > this->hp ) {
		int n = this->cards_size - this->hp;
		int *indexes = Player_chooseDrop(this->player, game, this->cards, this->cards_size, n, false);
		for ( int i=n-1; i>=0; i-- ) { // Don't change this!!
			trash = Avatar_taken(this, game, indexes[i]);
			Avatar_discard( game, trash);
			MESSAGE_PRINT("%s discard the card %s",this->player->username,trash->name);
		}
		free(indexes);
	}
}

int Avatar_onReact(Avatar *this, Game *game, int card_id, Card* to_react) {
	// fin TODO: Character ability - Jourdonnais
	// fin TODO: Equipment - Barrel 
	// TODO: Character ability - Sid Ketchum
	DEBUG_PRINT( "Ask Avatar %d to respond for %s Remain: %d\n", this->id, to_react ? to_react->name : "(NULL)" ,this->cards_size);
	if( this->equipment->armour != NULL  && to_react && to_react->id == CARD_BANG) {
		MESSAGE_PRINT("Using Barrel's ability");
		if( Avatar_judge(this,game,CARD_BARREL) == 0) {
			MESSAGE_PRINT("Lucky,is heart!");
			return 0;
		}else {
			MESSAGE_PRINT("uh oh! is not heart!");
		}
	}
	if( this->character->id == Jourdonnais && to_react && to_react->id == CARD_BANG ) {
		MESSAGE_PRINT("%s using his ability(%s).",this->player->username,this->character->name);
		if( Avatar_judge(this,game,CARD_BARREL) == 0) {
			MESSAGE_PRINT("Lucky,is heart!");
			return 0;
		}else {
			MESSAGE_PRINT("uh oh! is not heart!");
		}
	}

	// check which card can react
	int orgCards_size = this->cards_size;
	bool *validReact = malloc(orgCards_size * sizeof(bool));
	for ( int i=0; i<orgCards_size; i++ ) {
		validReact[i] = false;
		if ( this->cards[i]->id == card_id ) {
			validReact[i] = true;
		} else if ( this->character->id == Calamity_Janet && 
				( ( this->cards[i]->id == CARD_BANG && card_id == CARD_MISS ) ||
				( this->cards[i]->id == CARD_MISS && card_id == CARD_BANG ) ) ) {
			validReact[i] = true;
		}
	}

	// any card can react?
	bool any = false;
	for ( int i=0; i<orgCards_size; i++ ) {
		if ( validReact[i] ) {
			any = true;
			break;
		}
	}

	bool canUseAbility = false;
	if ( this->character->id == Sid_Ketchum && this->hp < this->hp_max && this->cards_size >= 2 ) canUseAbility = true;
	// Ask which card he want react
	int react;
	while ( 1 ) {
		react = Player_selectReact(this->player, game, this->cards, validReact, this->cards_size, canUseAbility);
		if ( react < -2 || react >= this->cards_size ) ERROR_PRINT("Invalid react.");
		if ( react == -1 || validReact[react] ) break;
		if ( canUseAbility && react == -2 ) {
			if ( this->character->id == Sid_Ketchum ) {
				int *rets = Player_chooseDrop(this->player, game, this->cards, this->cards_size, 2, true);
				if ( rets != NULL ) {
					Card* trash;
					MESSAGE_PRINT("%s use his ability(%s).",this->player->username,this->character->name);
					for ( int i=1; i>=0; i-- ) { // Don't change this!
						trash = Avatar_taken(this, game, rets[i]);
						Avatar_discard(game, trash);
						MESSAGE_PRINT("%s discard the card %s",this->player->username,trash->name);
					}
					Avatar_heal(this, game);
					free(rets);
				}
				free(validReact);
				return Avatar_onReact(this, game, card_id, to_react);
			}
		}
		WARNING_PRINT("You can't react with this card !");
	}
	// react
	if ( react == -1 ) {
		return -1;
	} else {
		Card *reactCard = this->cards[react];
		Avatar_discard(game, Avatar_taken(this, game, react));
	}
	free(validReact);
	if( (this->character->id == Jourdonnais || this->equipment->armour != NULL) && to_react && to_react->id == CARD_BANG ) {
		MESSAGE_PRINT("But %s has a MISS!",this->player->username);
	}
	return 0;
}

int Avatar_judge(Avatar *this, Game *game, int card_id) {
	Card *card ;
	if(this->character->id == Lucky_Duke) {
		Card **options = calloc(2,sizeof(Card));
		for(int i = 0; i<2; i++) {
			options[i] = Deck_draw(game->deck);
		}
		MESSAGE_PRINT("%s use his ability(%s) to choose judge card:",this->player->username,this->character->name);
		MESSAGE_PRINT("1.%s[%s]",options[0]->name,interface_getCardSuit(options[0]->suit));
		MESSAGE_PRINT("2.%s[%s]",options[1]->name,interface_getCardSuit(options[1]->suit));
		int*choosen = Avatar_choose(this,game,options,2,1, false);
		for(int i = 0; i<2 ; i++) {
			if ( i != choosen[0]) {
				Avatar_discard(game,options[i]);
			}else {
				card = options[i];
				Avatar_discard(game,options[i]);
			}
		}
		MESSAGE_PRINT("%s choose %s[%s]",this->player->username,card->name,interface_getCardSuit(card->suit));
		free(options);
	}else {
		card = Deck_draw(game->deck);
		Avatar_discard(game, card);
		MESSAGE_PRINT("The suit is %s.",interface_getCardSuit(card->suit));
	}
	if( card_id == CARD_DYNAMITE) {
		if ( card->suit >= 1 && card->suit <= 8 ) {
			MESSAGE_PRINT("BOOOOOOOOM!!!!");
			return 0;
		}else {
			MESSAGE_PRINT("Safe~");
			return -1;
		}
	}
	if( card_id == CARD_JAIL || card_id == CARD_BARREL ) { 
		if ( card->suit >= 13 && card->suit <= 25 ) {
			if(card_id == CARD_JAIL) MESSAGE_PRINT("Lucky!%s leave the jail.",this->player->username);
			return 0;
		}else {
			if(card_id == CARD_JAIL) MESSAGE_PRINT("uh oh!%s is so bad that he need to stay in jail.",this->player->username);
			return -1;
		}
	}
	interface_refresh(this->player->username, game);
	return 0;
}
void Avatar_dead(Avatar *this, Game *game) {
	// TODO: Character ability - Vulture Sam
	//discard cards
	MESSAGE_PRINT("%s(%s) is dead",this->player->username, print_role(this->role));
	Avatar* next = Game_nextAvailableAvatar(this);
	int check = -1;
	do {
		if(next->character->id == Vulture_Sam) {
			check = 0;
			break;
		}
		next = Game_nextAvailableAvatar(next);
	}while(next->character->id != this->character->id) ;
	this->isDead = true;
	if( check == 0 ) {
		MESSAGE_PRINT("Because %s's ability(%s),he get all cards and equipment from %s",next->player->username,next->character->name,this->player->username);
		for( int i = this->cards_size -1; i >= 0 ; i-- ) {
			Avatar_get(next,game,Avatar_taken(this, game, i));
		}
	}else {
		for( int i = this->cards_size -1; i >= 0 ; i-- ) {
			Avatar_discard( game, this->cards[i] );
			Avatar_taken(this, game, i);
			MESSAGE_PRINT("The card %s had been discard",this->cards[i]->name);
		}
		this->cards_size = 0;
	}
	//discard equipment
	if( this->equipment->armour != NULL) {
		if( check == 0) {
			Avatar_get(next,game,Avatar_unequip(this,game,&(this->equipment->armour)));
		}
		else {
			Avatar_discard( game,Avatar_unequip(this,game,&(this->equipment->armour)));
		}
		
	}
	if( this->equipment->horseMinus != NULL) {
		if( check == 0) {
			Avatar_get(next,game,Avatar_unequip(this,game,&(this->equipment->horseMinus)));
		}
		else {
			Avatar_discard( game,Avatar_unequip(this,game,&(this->equipment->horseMinus)));
		}
		
	}
	if( this->equipment->horsePlus != NULL) {
		if( check == 0) {
			Avatar_get(next,game,Avatar_unequip(this,game,&(this->equipment->horsePlus)));
		}
		else {
			Avatar_discard( game,Avatar_unequip(this,game,&(this->equipment->horsePlus)));
		}
		
	}
	if( this->equipment->gun != NULL) {
		if( check == 0) {
			Avatar_get(next,game,Avatar_unequip(this,game,&(this->equipment->gun)));
		}
		else {
			Avatar_discard( game,Avatar_unequip(this,game,&(this->equipment->gun)));
		}
		
	}
	if( this->equipment->bomb != NULL) {
		if( check == 0) {
			Avatar_get(next,game,Avatar_unequip(this,game,&(this->equipment->bomb)));
		}
		else {
			Avatar_discard( game,Avatar_unequip(this,game,&(this->equipment->bomb)) );
		}
		
	}
	if( this->equipment->jail != NULL) {
		if( check == 0) {
			Avatar_get(next,game,Avatar_unequip(this,game,&(this->equipment->jail)));
		}
		else {
			Avatar_discard( game, Avatar_unequip(this,game,&(this->equipment->jail)) );
		}
		
	}
	// TODO: Show role card
	//move dead people out
	
	game->numAvailableAvatar--;
	interface_refresh(this->player->username, game);
	Game_checkWin(game);
	
}

void Avatar_hurt(Avatar *this, Game *game, Avatar *attacker){
	// TODO: Character ability - Bart Cassidy 
	// TODO: Character ability - El Gringoy
	if(this->isDead == true) return;
	this->hp -- ;
	MESSAGE_PRINT("%s's hp -1.",this->player->username);
	if(this->hp == 0) {
		MESSAGE_PRINT("Oh no %s's hp equal 0",this->player->username);
		if( Avatar_onReact(this, game, CARD_BEER, NULL) == -1 || game->numAvailableAvatar <= 2) {
			if( attacker != NULL && this->role == OUTLAW) {
				MESSAGE_PRINT("%s kill %s,and he is outlaw,%s get three cards!",attacker->player->username,this->player->username,attacker->player->username);
				Avatar_draw(attacker,game);
				Avatar_draw(attacker,game);
				Avatar_draw(attacker,game);
			}
			if( attacker != NULL && this->role == DEPUTY && attacker->role == SHERIFF) {
				MESSAGE_PRINT("OH NO! %s kill %s, How can this Sheriff so cruel ? He is your Deputy ! Discard all your card ! NOW !!!",attacker->player->username,this->player->username);
				for( int i = attacker->cards_size -1; i >= 0 ; i-- ) {
					Avatar_discard( game, attacker->cards[i] );
					Avatar_taken(attacker, game, i);
					MESSAGE_PRINT("The card %s had been discard",this->cards[i]->name);
				}
			}
			Avatar_dead(this, game);
			return;
		}else {
			this->hp ++ ;
			MESSAGE_PRINT("but he use beer to heal himself");
		}
	}
	if( this->character->id == Bart_Cassidy) {
		MESSAGE_PRINT("%s hurt, using his ability(%s).",this->player->username,this->character->name);
		Avatar_draw(this,game);
	}
	if( attacker && this->character->id == El_Gringo && attacker->cards_size > 0) {
		MESSAGE_PRINT("%s hurt, using his ability(%s).",this->player->username,this->character->name);
		Card **list = malloc( attacker->cards_size * sizeof(Card *) );
		memset(list, 0, attacker->cards_size * sizeof(Card *));
		int *choose = Avatar_choose(this,game,list,attacker->cards_size,1, false);
		Avatar_get(this,game,Avatar_taken(attacker,game,choose[0]));
		free(list);
	}
	DEBUG_PRINT("Avatar %d hurt.\n", this->id);
	interface_refresh(this->player->username, game);
	return;
}

void Avatar_heal(Avatar *this, Game *game){
	this->hp ++;
	MESSAGE_PRINT("%s's hp +1",this->player->username);
	DEBUG_PRINT("Avatar %d heal.\n", this->id);
	interface_refresh(this->player->username, game);
	return;
}

void Avatar_equip(Avatar *this, Game *game, Card *card) {
	if( card->id > CARD_ARMOUR_START && card->id < CARD_ARMOUR_END ) {
		if( card->id == CARD_BARREL ) {
			if( this->equipment->armour != NULL) {
				Card *trash = Avatar_unequip(this,game,&(this->equipment->armour));
				Avatar_discard(game,trash);
				MESSAGE_PRINT("Because %s equipped the same equipment,the previous card had been discard!",this->player->username);
			}
			this->equipment->armour = card;
		}else if ( card->id == CARD_APPALOOSA ) {
			if( this->equipment->horseMinus != NULL) {
				Card *trash = Avatar_unequip(this,game,&(this->equipment->horseMinus));
				Avatar_discard(game,trash);
				MESSAGE_PRINT("Because %s equipped the same equipment,the previous card had been discard!",this->player->username);
			}
			this->equipment->horseMinus = card;
		}else if ( card->id == CARD_MUSTANG ) {
			if( this->equipment->horsePlus != NULL) {
				Card *trash = Avatar_unequip(this,game,&(this->equipment->horsePlus));
				Avatar_discard(game,trash);
				MESSAGE_PRINT("Because %s equipped the same equipment,the previous card had been discard!",this->player->username);
			}
			this->equipment->horsePlus = card;
		}
	}else if ( card->id > CARD_GUN_START && card->id < CARD_GUN_END ) {
		if( this->equipment->gun != NULL) {
			Card *trash = Avatar_unequip(this,game,&(this->equipment->gun));
			Avatar_discard(game,trash);
			MESSAGE_PRINT("Because %s can only have one gun,the previous card had been discard!",this->player->username);
		}
		this->equipment->gun = card;
	}else if( card->id == CARD_JAIL ) {
		this->equipment->jail = card;
	}else if ( card->id == CARD_DYNAMITE ) {
		if( this->equipment->bomb != NULL) {
			Card *trash = Avatar_unequip(this,game,&(this->equipment->bomb));
			Avatar_discard(game,trash);
			MESSAGE_PRINT("Because %s equipped the same equipment,the previous card had been discard!",this->player->username);
		}
		this->equipment->bomb = card;
	}
	DEBUG_PRINT("Avatar %d equipped the card: %s.\n", this->id , card->name);
	interface_refresh(this->player->username, game);
	return;
}

Card* Avatar_unequip(Avatar *this, Game *game, Card **card){
	Card *bye = *card;
	DEBUG_PRINT("%s unquipped the card: %s.", this->player->username, (*card)->name );
	*card = NULL;
	interface_refresh(this->player->username, game);
	return bye;
}

void Avatar_draw(Avatar *this, Game *game){
	Avatar_get(this,game, Deck_draw(game->deck));
	DEBUG_PRINT("Avatar %d draw one card.Remain: %d cards.\n", this->id ,game->deck->top + 1);
	interface_refresh(this->player->username, game);
	return;
}

int* Avatar_choose(Avatar *this, Game *game, Card **options , int size, int num, bool undo) {
	DEBUG_PRINT("%s start to choose\n",this->player->username);
	interface_refresh(this->player->username, game);
	return Player_chooseTake( this->player, game, options, size, num, undo);
}

void Avatar_get(Avatar *this, Game *game, Card *want){
	this->cards_size ++;
	this->cards[this->cards_size - 1] = want;
	DEBUG_PRINT("Avatar %d get the card: %s.\n", this->id , want->name);
	interface_refresh(this->player->username, game);
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
		MESSAGE_PRINT("%s have no card! Using his ability(%s).",this->player->username,this->character->name);
	}
	DEBUG_PRINT("Avatar %d's card: %s had been taken.\n", this->id , bye->name );
	interface_refresh(this->player->username, game);
	return bye;
}

int Avatar_calcDist(Game *game, Avatar *this, Avatar *that) {
	int dist1 = 0;
	Avatar *trav = this;
	while ( trav->id != that->id ) {
		trav = Game_nextAvailableAvatar(trav);
		dist1++;
	}
	int dist2 = 0;
	while ( trav->id != this->id ) {
		trav = Game_nextAvailableAvatar(trav);
		dist2++;
	}
	int dist = MIN(dist1, dist2);

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
		if ( *iter ) {
			list[this->cards_size+counter] = *iter;
			counter++;
		}
	}
	return list;
}

void Avatar_discard(Game*game, Card* card) {
	Deck_put(game->discardPile,card);
}
