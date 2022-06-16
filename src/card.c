#include <stdlib.h>
#include <string.h>

#include "card.h"
#include "debug.h"
#include "avatar.h"
#include "cardid.h"
#include "characterid.h"

Card *Card_init(const int id, const char *name, const int type, const int suit, int (*fun)(Avatar *, Avatar *, Game *, Card *)) {
	// TODO: card -> play

	if ( id <= CARD_START || id >= CARD_END ) ERROR_PRINT("Invalid card id %d\n", id);
	if(name == NULL) ERROR_PRINT("NULL pointer Mother Fucker !!\n");
	if(suit < 0 && suit >= 52) ERROR_PRINT("suit must within [0, 51]\n");

	Card *new = malloc(sizeof(Card));

	new->id = id;
	new->name = malloc(strlen(name)+1);
	strcpy(new->name, name);
	new->type = type;
	new->suit = suit;
	new->play = fun;
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
	wprintw(messgWin, "%s use BANG! to %s,",user->player->username,target->player->username);
	wrefresh(messgWin);
	if( Avatar_onReact(target, game, CARD_MISS, card ) == -1 ) {
		wprintw(messgWin, "%s do not have MISS,",target->player->username);
		Avatar_hurt(target, game, user);
	}else {
		if( user->character->id == Slab_the_Killer ) {
			wprintw(messgWin, "Because %s's ability,you need another MISSED!",user->player->username);
			if( Avatar_onReact(target, game, CARD_MISS, card ) == -1 ) {
				wprintw(messgWin, "%s do not have another MISS,",target->player->username);
				Avatar_hurt(target, game, user);
			}else {
				wprintw(messgWin, "but he missed!");
			}
	moveCurDown(messgWin);
		}else {
			wprintw(messgWin, "but he missed!");
		}
	}
	moveCurDown(messgWin);
	wrefresh(messgWin);
	interface_erase();
	interface_draw(user->player->username, game);
	return 0;
}
int play_CARD_MISS(Avatar * user, Avatar * target, Game * game, Card * card) {
	return -1;
}
int play_CARD_GATLING(Avatar * user, Avatar * target, Game * game, Card * card) {
	wprintw(messgWin, "%s use %s,",user->player->username,card->name);
	Avatar* next = Game_nextAvailableAvatar(user);
	while(next->id != user->id) 
	{
		if( Avatar_onReact( next, game, CARD_MISS, card ) == -1 ) 
		{
			wprintw(messgWin, "%s do not react with MISSED!,",next->player->username);
			Avatar_hurt(next, game, user);	
		}
		else
		{
			wprintw(messgWin, "%s react with MISSED!,nothing happened.",next->player->username);
		}
		moveCurDown(messgWin);
		next = Game_nextAvailableAvatar(next);
	}
	wrefresh(messgWin);
	return 0;
}
int play_CARD_INDIANS(Avatar * user, Avatar * target, Game * game, Card * card) {
	wprintw(messgWin, "%s use %s,",user->player->username,card->name);
	moveCurDown(messgWin);
	interface_erase();
	interface_draw(user->player->username, game);
	Avatar* next = Game_nextAvailableAvatar(user);
	while(next->id != user->id) 
	{
		if( Avatar_onReact( next, game, CARD_BANG, card ) == -1 ) 
		{
			wprintw(messgWin, "%s do not react with BANG!,",next->player->username);
			Avatar_hurt(next, game, user);	
		}
		else
		{
			wprintw(messgWin, "%s react with BANG!,nothing happened.",next->player->username);
		}
		moveCurDown(messgWin);
		next = Game_nextAvailableAvatar(next);
	}
	interface_erase();
	interface_draw(user->player->username, game);
	wrefresh(messgWin);
	return 0;
}
int play_CARD_PANIC(Avatar * user, Avatar * target, Game * game, Card * card) {
	//TODO:Choose equipment
	wprintw(messgWin, "%s use %s",user->player->username,card->name);
	moveCurDown(messgWin);
	wrefresh(messgWin);
	int *choose = Avatar_choose(user,game,target->cards,target->cards_size,1);
	Avatar_get(user,game,Avatar_taken(target, game, choose[0]));
	return 0;
}
int play_CARD_BALOU(Avatar * user, Avatar * target, Game * game, Card * card) {
	//TODO:Choose equipment
	wprintw(messgWin, "%s use %s",user->player->username,card->name);
	moveCurDown(messgWin);
	wrefresh(messgWin);
	int *choose = Avatar_choose(user,game,target->cards,target->cards_size,1);
	Card* trash = Avatar_taken(target,game,choose[0]);
	Deck_put(game->discardPile,trash);
	return 0;
}
int play_CARD_STAGECOACH(Avatar * user, Avatar * target, Game * game, Card * card) {
	interface_erase();
	interface_draw(user->player->username, game);
	wprintw(messgWin, "%s use %s",user->player->username,card->name);
	moveCurDown(messgWin);
	wrefresh(messgWin);
	Avatar_draw(user,game);
	Avatar_draw(user,game);
	return 0;
}
int play_CARD_FARGO(Avatar * user, Avatar * target, Game * game, Card * card) {
	interface_erase();
	interface_draw(user->player->username, game);
	wprintw(messgWin, "%s use %s",user->player->username,card->name);
	moveCurDown(messgWin);
	wrefresh(messgWin);
	Avatar_draw(user,game);
	Avatar_draw(user,game);
	Avatar_draw(user,game);
	return 0;
}
int play_CARD_STORE(Avatar * user, Avatar * target, Game * game, Card * card) {
	interface_erase();
	interface_draw(user->player->username, game);
	wprintw(messgWin, "%s use %s",user->player->username,card->name);
	moveCurDown(messgWin);
	wrefresh(messgWin);
	Avatar* next = user;
	Card** options = calloc(4,sizeof(Card));
	int* choose = calloc(4,1);
	int opt_size = game->numAvailableAvatar;
	for(int i = 0; i<opt_size;i++) {
		options[i] = Deck_draw(game->deck);
	}
	do{
		choose = Avatar_choose(next,game,options,opt_size,1);	
		Avatar_get(next,game,options[choose[0]]);
		for(int i = choose[0];i < opt_size - 1 ; i++) {
			options[i] = options[i+1];
		}
		opt_size --;
		next = Game_nextAvailableAvatar(next);
	}while(next->id != user->id);
	return 0;
}
int play_CARD_BEER(Avatar * user, Avatar * target, Game * game, Card * card) {
	wprintw(messgWin, "%s use %s",user->player->username,card->name);
	moveCurDown(messgWin);
	wrefresh(messgWin);
	Avatar_heal(user, game);
	return 0;
}
int play_CARD_SALOON(Avatar * user, Avatar * target, Game * game, Card * card) {
	wprintw(messgWin, "%s use %s",user->player->username,card->name);
	wrefresh(messgWin);
	Avatar* next = user;
	do {
		if( next->hp != next->hp_max) {
			Avatar_heal(next,game);
		}
		else {
			wprintw(messgWin, "%s hp is max",next->player->username);
			moveCurDown(messgWin);
			wrefresh(messgWin);
		}
		next = Game_nextAvailableAvatar(next);
	}while(next->id != user->id);
	return 0;
}
int play_CARD_DUEL(Avatar * user, Avatar * target, Game * game, Card * card) {
	interface_erase();
	interface_draw(user->player->username, game);
	wprintw(messgWin, "%s use %s to %s",user->player->username,card->name,target->player->username);
	wrefresh(messgWin);
	while(1) {
		if( Avatar_onReact(target, game, CARD_BANG, card) == -1) {
			wprintw(messgWin, "%s lose the duel,",target->player->username);
			Avatar_hurt(target, game, user);
			return 0;
		}else {
			wprintw(messgWin, "%s has a Bang!",target->player->username);
			wrefresh(messgWin);
			if( Avatar_onReact(user, game, CARD_BANG, card) == -1) {
				wprintw(messgWin, "%s lose the duel,",user->player->username);
				Avatar_hurt(user, game, target);
				return 0;
			}else {
				wprintw(messgWin, "%s has a Bang!",user->player->username);
				continue;
			}
		}
		moveCurDown(messgWin);
		wrefresh(messgWin);
	}

}
int play_CARD_BARREL(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	interface_erase();
	interface_draw(user->player->username, game);
	wprintw(messgWin, "%s equipped the %s",user->player->username,card->name);
	moveCurDown(messgWin);
	wrefresh(messgWin);
	return 0;
}
int play_CARD_SCOPE(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	interface_erase();
	interface_draw(user->player->username, game);
	wprintw(messgWin, "%s equipped the %s",user->player->username,card->name);
	moveCurDown(messgWin);
	wrefresh(messgWin);
	return 0;
}
int play_CARD_MUSTANG(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	interface_erase();
	interface_draw(user->player->username, game);
	wprintw(messgWin, "%s equipped the %s",user->player->username,card->name);
	moveCurDown(messgWin);
	wrefresh(messgWin);
	return 0;
}
int play_CARD_VOLCANIC(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	interface_erase();
	interface_draw(user->player->username, game);
	wprintw(messgWin, "%s equipped the %s",user->player->username,card->name);
	moveCurDown(messgWin);
	wrefresh(messgWin);
	return 0;
}
int play_CARD_SCHOFIELD(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	interface_erase();
	interface_draw(user->player->username, game);
	wprintw(messgWin, "%s equipped the %s",user->player->username,card->name);
	moveCurDown(messgWin);
	wrefresh(messgWin);
	return 0;
}
int play_CARD_REMINGTON(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	interface_erase();
	interface_draw(user->player->username, game);
	wprintw(messgWin, "%s equipped the %s",user->player->username,card->name);
	moveCurDown(messgWin);
	wrefresh(messgWin);
	return 0;
}
int play_CARD_CARABINE(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	interface_erase();
	interface_draw(user->player->username, game);
	wprintw(messgWin, "%s equipped the %s",user->player->username,card->name);
	moveCurDown(messgWin);
	wrefresh(messgWin);
	return 0;
}
int play_CARD_WINCHEDTER(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	interface_erase();
	interface_draw(user->player->username, game);
	wprintw(messgWin, "%s equipped the %s",user->player->username,card->name);
	moveCurDown(messgWin);
	wrefresh(messgWin);
	return 0;
}
int play_CARD_JAIL(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( target, game, card );
	interface_erase();
	interface_draw(user->player->username, game);
	wprintw(messgWin, "%s use %s to %s",user->player->username,card->name,target->player->username);
	moveCurDown(messgWin);
	wrefresh(messgWin);
	return 0;
}
int play_CARD_DYNAMITE(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	interface_erase();
	interface_draw(user->player->username, game);
	wprintw(messgWin, "%s use a %s",user->player->username,card->name);
	moveCurDown(messgWin);
	wrefresh(messgWin);
	return 0;
}
