#include <stdlib.h>
#include <string.h>

#include "card.h"
#include "debug.h"
#include "avatar.h"
#include "cardid.h"

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

void Card_free(Card *this) {
	free(this->name);
	free(this);
}

// int Card_playBang(Avatar * user, Avatar * target, Game * game, Card * card) {
// 	if ( Avatar_onReact( target , game , card ) == 0 ){
// 		target->hp -- ;
// 	}
// 	return 0;
// }

// int Card_playMiss(Avatar * user, Avatar * target, Game * game, Card * card) {
// 	return 0;
// }

// int Card_playBeer(Avatar * user, Avatar * target, Game * game, Card * card) {
// 	user->hp ++ ;
// 	return 0;
// }

// int Card_playBeer(Avatar * user, Avatar * target, Game * game, Card * card) {
// 	user->hp ++ ;
// 	return 0;
// }

// int Card_playSallon(Avatar * user, Avatar * target, Game * game, Card * card) {
// 	for(size_t i = 0 ; i < game->numAvatar ; i++)
// 		game->avatars[i]->hp ++ ;
// 	return 0;
// }


// int Card_playWellsFargo(Avatar * user, Avatar * target, Game * game, Card * card) {
// 	Game_Draw(game, user, 3);
// 	return 0;
// }

// int Card_playStagecoach(Avatar * user, Avatar * target, Game * game, Card * card) {
// 	Game_DrawTo(game, user, 2);
// 	return 0;
// }

// int Card_playStagecoach(Avatar * user, Avatar * target, Game * game, Card * card) {
// 	Game_DrawTo(game, user, 2);
// 	return 0;
// }

// int Card_playGeneralStore(Avatar * user, Avatar * target, Game * game, Card * card) {
// 	Game_Draw(game, user, game->numAvatar);
// 	return 0;
// }

// int Card_playPanic(Avatar * user, Avatar * target, Game * game, Card * card) {
// 	Game_Draw(game, user, game->numAvatar);
// 	return 0;
// }

// int Card_playGatling(Avatar * user, Avatar * target, Game * game, Card * card) {
// 	if ( Avatar_onReact( target , game , card ) == 0 ){
// 		target->hp -- ;
// 	}
// 	return 0;
// }

int play_CARD_BANG(Avatar * user, Avatar * target, Game * game, Card * card) {
	printf("%s use BANG! to %s,",user->player->username,target->player->username);
	if( Avatar_onReact(target, game, CARD_MISS) == -1 ) {
		printf("%s do not have MISS,",target->player->username);
		Avatar_hurt(target, game, user);
	}else {
		printf("but he missed!\n");
	}
	return 0;
}
int play_CARD_MISS(Avatar * user, Avatar * target, Game * game, Card * card) {
	return -1;
}
int play_CARD_GATLING(Avatar * user, Avatar * target, Game * game, Card * card) {
	printf("%s use %s,",user->player->username,card->name);
	Avatar* next = Game_nextAvailableAvatar(user);
	while(next->id == user->id) 
	{
		if( Avatar_onReact( next, game, CARD_MISS ) == -1 ) 
		{
			printf("%s do not react with MISSED!,",next->player->username);
			Avatar_hurt(next, game, user);	
		}
		else
		{
			printf("%s react with MISSED!,nothing happened.\n",next->player->username);
		}
		next = Game_nextAvailableAvatar(next);
	}
	return 0;
}
int play_CARD_INDIANS(Avatar * user, Avatar * target, Game * game, Card * card) {
	printf("%s use %s,",user->player->username,card->name);
	Avatar* next = Game_nextAvailableAvatar(user);
	while(next->id != user->id) 
	{
		if( Avatar_onReact( next, game, CARD_BANG ) == -1 ) 
		{
			printf("%s do not react with BANG!,",next->player->username);
			Avatar_hurt(next, game, user);	
		}
		else
		{
			printf("%s react with BANG!,nothing happened.\n",next->player->username);
		}
		next = Game_nextAvailableAvatar(next);
	}
	return 0;
}
int play_CARD_PANIC(Avatar * user, Avatar * target, Game * game, Card * card) {
	//TODO:Choose equipment
	printf("%s use %s\n",user->player->username,card->name);
	int *choose = Avatar_choose(user,game,target->cards,target->cards_size,1);
	Avatar_get(user,game,target->cards[choose[0]]);
	return 0;
}
int play_CARD_BALOU(Avatar * user, Avatar * target, Game * game, Card * card) {
	//TODO:Choose equipment
	printf("%s use %s\n",user->player->username,card->name);
	int *choose = Avatar_choose(user,game,target->cards,target->cards_size,1);
	Avatar_taken(target,game,choose[0]);
	return 0;
}
int play_CARD_STAGECOACH(Avatar * user, Avatar * target, Game * game, Card * card) {
	printf("%s use %s\n",user->player->username,card->name);
	Avatar_draw(user,game);
	Avatar_draw(user,game);
	return 0;
}
int play_CARD_FARGO(Avatar * user, Avatar * target, Game * game, Card * card) {
	printf("%s use %s\n",user->player->username,card->name);
	Avatar_draw(user,game);
	Avatar_draw(user,game);
	Avatar_draw(user,game);
	return 0;
}
int play_CARD_STORE(Avatar * user, Avatar * target, Game * game, Card * card) {
	printf("%s use %s\n",user->player->username,card->name);
	Avatar* next = Game_nextAvailableAvatar(user);
	Card** options = calloc(4,sizeof(Card));
	for(int i = 0; i<game->numAvailablePlayer;i++) {
		options[i] = Deck_draw(game->deck);
	}
	int* choose = Avatar_choose(user,game,options,game->numAvailablePlayer,1);
	Avatar_get(user,game,options[choose[0]]);
	for(int i = choose[0];i < game->numAvailablePlayer - 1 ; i++) {
		options[i] = options[i+1];
	}
	while(next->id != user->id) {
		choose = Avatar_choose(next,game,options,game->numAvailablePlayer,1);	
		Avatar_get(next,game,options[choose[0]]);
		for(int i = choose[0];i < game->numAvailablePlayer - 1 ; i++) {
			options[i] = options[i+1];
		}
		next = Game_nextAvailableAvatar(next);
	}
	return 0;
}
int play_CARD_BEER(Avatar * user, Avatar * target, Game * game, Card * card) {
	if( game->numAvailablePlayer <= 2) {
		WARNING_PRINT("You can't use beer when only two player left!\n");
		return -1;
	}else if( user->hp >= user->hp_max) {
		WARNING_PRINT("Your hp is max!\n");
		return -1;
	}
	printf("%s use %s\n",user->player->username,card->name);
	Avatar_heal(user, game);
	return 0;
}
int play_CARD_SALOON(Avatar * user, Avatar * target, Game * game, Card * card) {
	printf("%s use %s\n",user->player->username,card->name);
	int check = -1;
	Avatar* next = Game_nextAvailableAvatar(user);
	if(user->hp == user->hp_max)
	{
		while(next->id != user->id) 
		{
			if( next->hp != next->hp_max) 
			{
				check = 0;
				break;
			}
			next = Game_nextAvailableAvatar(next);
		}
		if( check == -1)
		{
			WARNING_PRINT("Everyone hp is max!\n");
			return -1;
		}
	}
	else
	{
		Avatar_heal(user,game);
		while(next->id != user->id) 
		{
			if( next->hp != next->hp_max) 
			{
				Avatar_heal(next,game);
			}
			else 
			{
				printf("%s hp is max\n",next->player->username);
			}
			next = Game_nextAvailableAvatar(next);
		}
	}
	return 0;
}
int play_CARD_DUEL(Avatar * user, Avatar * target, Game * game, Card * card) {
	printf("%s use %s to %s\n",user->player->username,card->name,target->player->username);
	while(1) {
		if( Avatar_onReact(target, game, CARD_BANG) == -1) {
			printf("%s lose the duel,",target->player->username);
			Avatar_hurt(target, game, user);
			return 0;
		}else {
			printf("%s has a Bang!\n",target->player->username);
			if( Avatar_onReact(user, game, CARD_BANG) == -1) {
				printf("%s lose the duel,",user->player->username);
				Avatar_hurt(user, game, target);
				return 0;
			}else {
				printf("%s has a Bang!\n",user->player->username);
				continue;
			}
		}
	}

}
int play_CARD_BARREL(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	printf("%s equipped the %s\n",user->player->username,card->name);
	return 0;
}
int play_CARD_SCOPE(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	printf("%s equipped the %s\n",user->player->username,card->name);
	return 0;
}
int play_CARD_MUSTANG(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	printf("%s equipped the %s\n",user->player->username,card->name);
	return 0;
}
int play_CARD_VOLCANIC(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	printf("%s equipped the %s\n",user->player->username,card->name);
	return 0;
}
int play_CARD_SCHOFIELD(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	printf("%s equipped the %s\n",user->player->username,card->name);
	return 0;
}
int play_CARD_REMINGTON(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	printf("%s equipped the %s\n",user->player->username,card->name);
	return 0;
}
int play_CARD_CARABINE(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	printf("%s equipped the %s\n",user->player->username,card->name);
	return 0;
}
int play_CARD_WINCHEDTER(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	printf("%s equipped the %s\n",user->player->username,card->name);
	return 0;
}
int play_CARD_JAIL(Avatar * user, Avatar * target, Game * game, Card * card) {
	if ( target->role == SHERIFF ) {
		WARNING_PRINT("You can not use jail to sheriff!\n");
		return -1;
	}else if ( target->equipment->jail != NULL) {
		WARNING_PRINT("The target you choose had already in jail!\n");
		return -1;
	}else {
		Avatar_equip( target, game, card );
		printf("%s use %s to %s\n",user->player->username,card->name,target->player->username);
		return 0;
	} 
}
int play_CARD_DYNAMITE(Avatar * user, Avatar * target, Game * game, Card * card) {
	Avatar_equip( user, game, card );
	printf("%s use a %s\n",user->player->username,card->name);
	return 0;
}
