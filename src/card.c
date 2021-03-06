#include <stdlib.h>
#include <string.h>

#include "card.h"
#include "print.h"
#include "avatar.h"
#include "cardid.h"
#include "characterid.h"

Card *Card_init(const int id, const char *name, const int type, const int suit, const char *intro, int (*fun)(Avatar *, Avatar *, Game *, Card *)) {
	// TODO: card -> play

	if ( id <= CARD_START || id >= CARD_END ) ERROR_PRINT("Invalid card id %d", id);
	if(name == NULL) ERROR_PRINT("NULL pointer!!");
	if(suit < 0 && suit >= 52) ERROR_PRINT("suit must within [0, 51]");

	Card *new = malloc(sizeof(Card));

	new->id = id;
	new->name = malloc(strlen(name)+1);
	strcpy(new->name, name);
	new->type = type;
	new->suit = suit;
	new->play = fun;
	new->intro = malloc(strlen(intro) + 1);
	strcpy(new->intro, intro);
	return new;
}

Card *Card_copy(Card *this) {
	Card *new = malloc(sizeof(Card));
	new->id = this->id;
	new->name = malloc(strlen(this->name)+1);
	strcpy(new->name, this->name);
	new->type = this->type;
	new->suit = this->suit;
	new->play = this->play;
	return new;
}

void Card_free(Card *this) {
	if ( this==NULL ) return;
	free(this->name);
	free(this);
}

int play_CARD_BANG(Avatar * user, Avatar * target, Game * game, Card * card) {
	MESSAGE_PRINT("%s use BANG! to %s.",user->player->username,target->player->username);
	if( Avatar_onReact(target, game, CARD_MISS, card ) == -1 ) {
		MESSAGE_PRINT("%s do not react with MISS.",target->player->username);
		Avatar_hurt(target, game, user);
	}else {
		if( user->character->id == Slab_the_Killer ) {
			MESSAGE_PRINT("Because %s's ability(%s),%s need another MISSED!",user->player->username,user->character->name,target->player->username);
			if( Avatar_onReact(target, game, CARD_MISS, NULL ) == -1 ) {
				MESSAGE_PRINT("%s do not react with another MISS.",target->player->username);
				Avatar_hurt(target, game, user);
			}else {
				MESSAGE_PRINT("%s missed!",user->player->username);
			}
		}else {
			MESSAGE_PRINT("%s missed!",user->player->username);
		}
	}
	return 0;
}
int play_CARD_MISS(Avatar * user, Avatar * target, Game * game, Card * card) {
	return -1;
}
int play_CARD_GATLING(Avatar * user, Avatar * target, Game * game, Card * card) {
	MESSAGE_PRINT("%s use %s.",user->player->username,card->name);
	Avatar* next = Game_nextAvailableAvatar(user);
	while(next->id != user->id) 
	{
		if( Avatar_onReact( next, game, CARD_MISS, card ) == -1 ) 
		{
			MESSAGE_PRINT("%s do not react with MISSED!",next->player->username);
			Avatar_hurt(next, game, user);	
		}
		else
		{
			MESSAGE_PRINT("%s react with MISSED!,nothing happened.",next->player->username);
		}
		next = Game_nextAvailableAvatar(next);
	}
	return 0;
}
int play_CARD_INDIANS(Avatar * user, Avatar * target, Game * game, Card * card) {
	MESSAGE_PRINT("%s use %s.",user->player->username,card->name);
	Avatar* next = Game_nextAvailableAvatar(user);
	while(next->id != user->id) 
	{
		if( Avatar_onReact( next, game, CARD_BANG, card ) == -1 ) 
		{
			MESSAGE_PRINT("%s do not react with BANG!",next->player->username);
			Avatar_hurt(next, game, user);	
		}
		else
		{
			MESSAGE_PRINT("%s react with BANG!,nothing happened.",next->player->username);
		}
		next = Game_nextAvailableAvatar(next);
	}
	return 0;
}
int play_CARD_PANIC(Avatar * user, Avatar * target, Game * game, Card * card) {
	//TODO:Choose equipment
	MESSAGE_PRINT("%s use %s to %s.",user->player->username,card->name,target->player->username);
	int list_size;
	Card **list = Avatar_giveToChoose(target, &list_size);
	DEBUG_PRINT("%s ,list generated Done\n",card->name);
	int *choose = Avatar_choose(user,game,list,list_size,1, false);
	if ( choose[0] < target->cards_size ) {
		Avatar_get(user,game,Avatar_taken(target, game, choose[0]));
	} else {
		int idx = target->cards_size;
		for ( Card ** iter = (Card **)target->equipment; iter < (Card **)(target->equipment+1); iter++) {
			if ( *iter && choose[0] == idx++ ) {
				Avatar_get(user,game,Avatar_unequip(target, game, iter));
				break;
			}
		}
	}
	free(list);
	return 0;
}
int play_CARD_BALOU(Avatar * user, Avatar * target, Game * game, Card * card) {
	//TODO:Choose equipment
	MESSAGE_PRINT("%s use %s to %s.",user->player->username,card->name,target->player->username);
	int list_size;
	Card **list = Avatar_giveToChoose(target, &list_size);
	DEBUG_PRINT("%s ,list generated Done\n",card->name);
	int *choose = Avatar_choose(user,game,list,list_size,1, false);
	DEBUG_PRINT("%s ,list choose Done\n",card->name);
	Card* trash;
	if ( choose[0] < target->cards_size ) {
		trash = Avatar_taken(target, game, choose[0]);
	} else {
		int idx = target->cards_size;
		for ( Card ** iter = (Card **)target->equipment; iter < (Card **)(target->equipment+1); iter++) {
			if ( *iter && choose[0] == idx++ ) {
				trash = Avatar_unequip(target, game, iter);
				break;
			}
		}
	}
	MESSAGE_PRINT("%s's card %s had been discard!",target->player->username,trash->name);
	Deck_put(game->discardPile,trash);
	free(list);
	return 0;
}
int play_CARD_STAGECOACH(Avatar * user, Avatar * target, Game * game, Card * card) {
	MESSAGE_PRINT("%s use %s.",user->player->username,card->name);
	Avatar_draw(user,game);
	Avatar_draw(user,game);
	return 0;
}
int play_CARD_FARGO(Avatar * user, Avatar * target, Game * game, Card * card) {
	MESSAGE_PRINT("%s use %s",user->player->username,card->name);
	Avatar_draw(user,game);
	Avatar_draw(user,game);
	Avatar_draw(user,game);
	return 0;
}
int play_CARD_STORE(Avatar * user, Avatar * target, Game * game, Card * card) {
	MESSAGE_PRINT("%s use %s.",user->player->username,card->name);
	Avatar* next = user;
	Card** options = calloc(game->numAvailableAvatar, sizeof(Card*));
	int* choose ;
	int opt_size = game->numAvailableAvatar;
	for(int i = 0; i<opt_size;i++) {
		options[i] = Deck_draw(game->deck);
	}
	do{
		choose = Avatar_choose(next,game,options,opt_size,1, false);
		Avatar_get(next,game,options[choose[0]]);
		MESSAGE_PRINT("%s get the card %s",next->player->username,options[choose[0]]->name);
		for(int i = choose[0];i < opt_size - 1 ; i++) {
			options[i] = options[i+1];
		}
		opt_size --;
		next = Game_nextAvailableAvatar(next);
	}while(next->id != user->id);
	free(options);
	return 0;
}
int play_CARD_BEER(Avatar * user, Avatar * target, Game * game, Card * card) {
	MESSAGE_PRINT("%s use %s",user->player->username,card->name);
	Avatar_heal(user, game);
	return 0;
}
int play_CARD_SALOON(Avatar * user, Avatar * target, Game * game, Card * card) {
	MESSAGE_PRINT("%s use %s",user->player->username,card->name);
	Avatar* next = user;
	do {
		if( next->hp != next->hp_max) {
			Avatar_heal(next,game);
		}
		else {
			MESSAGE_PRINT("%s hp is max",next->player->username);
		}
		next = Game_nextAvailableAvatar(next);
	}while(next->id != user->id);
	return 0;
}
int play_CARD_DUEL(Avatar * user, Avatar * target, Game * game, Card * card) {
	MESSAGE_PRINT("%s use %s to %s",user->player->username,card->name,target->player->username);
	while(1) {
		if( Avatar_onReact(target, game, CARD_BANG, card) == -1) {
			MESSAGE_PRINT("%s lose the duel.",target->player->username);
			Avatar_hurt(target, game, user);
			return 0;
		}else {
			MESSAGE_PRINT("%s has a Bang!",target->player->username);
			/* wrefresh(messgWin); */
			if( Avatar_onReact(user, game, CARD_BANG, card) == -1) {
				MESSAGE_PRINT("%s lose the duel.",user->player->username);
				Avatar_hurt(user, game, target);
				return 0;
			}else {
				MESSAGE_PRINT("%s has a Bang!",user->player->username);
				continue;
			}
		}
	}

}
int play_CARD_BARREL(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	MESSAGE_PRINT("%s equipped the %s",user->player->username,card->name);
	return 0;
}
int play_CARD_APPALOOSA(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	MESSAGE_PRINT("%s equipped the %s",user->player->username,card->name);
	return 0;
}
int play_CARD_MUSTANG(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	MESSAGE_PRINT("%s equipped the %s",user->player->username,card->name);
	return 0;
}
int play_CARD_VOLCANIC(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	MESSAGE_PRINT("%s equipped the %s",user->player->username,card->name);
	return 0;
}
int play_CARD_SCHOFIELD(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	MESSAGE_PRINT("%s equipped the %s",user->player->username,card->name);
	return 0;
}
int play_CARD_REMINGTON(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	MESSAGE_PRINT("%s equipped the %s",user->player->username,card->name);
	return 0;
}
int play_CARD_CARABINE(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	MESSAGE_PRINT("%s equipped the %s",user->player->username,card->name);
	return 0;
}
int play_CARD_WINCHEDTER(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	MESSAGE_PRINT("%s equipped the %s",user->player->username,card->name);
	return 0;
}
int play_CARD_JAIL(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( target, game, card );
	MESSAGE_PRINT("%s use %s to %s",user->player->username,card->name,target->player->username);
	return 0;
}
int play_CARD_DYNAMITE(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	MESSAGE_PRINT("%s use a %s",user->player->username,card->name);
	return 0;
}
