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

int computer_selectUse(Player *this, Game *game, Card **cards, bool *validCards, int cards_size) {
	DEBUG_PRINT("computer selecting.\n");
	usleep(500);
	for ( int i=0; i<cards_size; i++ ) {
		if ( validCards[i] ) {
			return i;
		}
	}
	DEBUG_PRINT("No card can be selected.\n");
	return -1;
}

int computer_selectTarget(Player *this, Game *game, bool *validTargets) {
	DEBUG_PRINT("computer selecting.\n");
	Avatar *self = this->avatar;
	int tarIdx = -1;
	for ( int i=0; i<game->numAvatar; i++ ) {
		bool isSheriff = game->avatars[i]->role == SHERIFF;
		if ( validTargets[i] ) {
			if ( isSheriff ) {
				if ( self->role == OUTLAW ) tarIdx = i;
			} else {
				if ( self->role != OUTLAW ) tarIdx = i;
			}
		}
	}
	if ( tarIdx != -1 ) return tarIdx;
	for ( int i=0; i<game->numAvatar; i++ ) {
		bool isSheriff = game->avatars[i]->role == SHERIFF;
		if ( validTargets[i] && isSheriff ) {
			tarIdx = i;
		}
	}
	if ( tarIdx != -1 ) return tarIdx;
	for ( int i=0; i<game->numAvatar; i++ ) {
		if ( validTargets[i] ) {
			tarIdx = i;
		}
	}
	if ( tarIdx == -1 ) ERROR_PRINT("No target can be choosed.\n");
	return tarIdx;
}

int computer_selectReact(Player *this, Game *game, Card **cards, bool *validReact, int cards_size) {
	for ( int i=0; i<cards_size; i++ ) {
		if ( validReact[i] ) {
			return i;
		}
	}
	return -1;
}

bool computer_useAbility(Player *this, Game *game) {
	return false;
}
