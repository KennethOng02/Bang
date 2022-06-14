#include <stdlib.h>
#include <string.h>

#include "card.h"
#include "debug.h"
#include "avatar.h"
#include "cardid.h"

Card *Card_init(const int id, const char *name, const int suit) {
	// TODO: card -> play

	if ( id <= CARD_START || id >= CARD_END ) ERROR_PRINT("Invalid card id %d\n", id);
	if(name == NULL) ERROR_PRINT("NULL pointer Mother Fucker !!\n");
	if(suit < 0 && suit >= 52) ERROR_PRINT("suit must within [0, 51]\n");

	Card *new = malloc(sizeof(Card));

	new->id = id;
	new->name = malloc(strlen(name)+1);
	strcpy(new->name, name);
	new->suit = suit;
	new->play = NULL;
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
