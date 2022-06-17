#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "player.h"
#include "game.h"
#include "card.h"
#include "print.h"

static int computerCounter = 0;
char *computer_askName() {
	computerCounter++;
	char *name = malloc(20);
	snprintf(name, 20, "Computer %d", computerCounter);
	return name;
}

int *computer_chooseTake(Player *this, Game *game, Card **cards, int cards_size, int n) {
	int *idxs = malloc(n * sizeof(int));	
	for ( int i=0; i<n; i++ ) {
		idxs[i] = i;
	}
	return idxs;
}

int *computer_chooseDrop(Player *this, Game *game, Card **cards, int cards_size, int n) {
	int *idxs = malloc(n * sizeof(int));	
	for ( int i=0; i<n; i++ ) {
		idxs[i] = i;
	}
	return idxs;
}

int computer_selectUse(Player *this, Game *game, Card **cards, int cards_size) {
	usleep(500);
	return rand() % (cards_size+1) - 1;
}

Player *computer_selectTarget(Player *this, Game *game) {
	Avatar *avatar = NULL;
	Player *target = NULL;
	for ( int i=0; i<game->numAvatar; i++ ) {
		if ( game->avatars[i]->id == this->id ) {
			avatar = game->avatars[i];
			break;
		}
	}
	Avatar *self = avatar;
	avatar = Game_nextAvailableAvatar(avatar);
	while ( avatar->id != this->id ) {

		Role role = avatar->role;

		switch ( self->role ) {
		
		case SHERIFF:
		case DEPUTY:
			if ( role != SHERIFF ) {
				target = avatar->player;
			}
			break;

		case OUTLAW:
			if ( role == SHERIFF ) {
				target = avatar->player;
			}
			break;

		case RENEGADE:
			if ( role != SHERIFF ) {
				target = avatar->player;
			} else if ( target == NULL ) {
				target = avatar->player;
			}
			break;

		default:
			ERROR_PRINT("Unknown role.\n");
		}
		avatar = Game_nextAvailableAvatar(avatar);
	}
	return target;
}

int computer_selectReact(Player *this, Game *game, Card **cards, int cards_size) {
	usleep(500);
	DEBUG_PRINT("here la\n");
	int ret = ( rand() % (cards_size+1) ) - 1;
	DEBUG_PRINT("here ba\n");
	return ret;
}

bool computer_useAbility(Player *this, Game *game) {
	return false;
}
