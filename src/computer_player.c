#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "player.h"
#include "game.h"
#include "card.h"
#include "debug.h"

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

int computer_selectUse(Player *this, Game *game, Card **cards, int cards_size, Player **target) {
	*target = NULL;
	Avatar *avatar = Game_nextAvailableAvatar(this->avatar);
	while ( avatar->id != this->avatar->id ) {

		Role role = avatar->role;

		switch ( this->avatar->role ) {
		
		case SHERIFF:
		case DEPUTY:
			if ( role != SHERIFF ) {
				*target = avatar->player;
			}
			break;

		case OUTLAW:
			if ( role == SHERIFF ) {
				*target = avatar->player;
			}
			break;

		case RENEGADE:
			if ( role != SHERIFF ) {
				*target = avatar->player;
			} else if ( *target == NULL ) {
				*target = avatar->player;
			}
			break;

		default:
			ERROR_PRINT("Unknown role.\n");
		}
		avatar = Game_nextAvailableAvatar(avatar);
		
	}
	return rand() % (cards_size+1) - 1;
}

int computer_selectReact(Player *this, Game *game, Card **cards, int cards_size) {
	srand(time(0));
	return rand() % (cards_size+1) - 1;
}

bool computer_useAbility(Player *this, Game *game) {
	return false;
}
