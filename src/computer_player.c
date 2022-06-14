#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "player.h"
#include "game.h"
#include "card.h"

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
	return 0;
}

int computer_selectReact(Player *this, Game *game, Card **cards, int cards_size) {
	return 0;
}

bool computer_useAbility(Player *this, Game *game) {
	return false;
}
