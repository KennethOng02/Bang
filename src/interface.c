#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "interface.h"
#include "player.h"
#include "game.h"
#include "card.h"
#include "debug.h"
#include "mylib.h"
#include "ANSI-color-codes.h"

void interface_printCards(Card **cards, int cards_size) {
	for ( int i=0; i<cards_size; i++ ) {
		printf("%d) %s\n", i+1, cards[i]->name);
	}
}

bool interface_yesOrNo(void) {
	int bufSize = 10;
	char buffer[bufSize];
	while ( true ) {
		printf("(y/n):");
		if ( fgets(buffer, bufSize, stdin) == NULL ) {
			WARNING_PRINT("Please enter 'y' or 'n'.\n");
			continue;
		}
		if ( strcmp(buffer, "y\n") == 0 ) return true;
		if ( strcmp(buffer, "n\n") == 0 ) return false;
	}
	return false;
}

int *interface_choose(Player *this, Game *game, Card **cards, int cards_size, int n, char *msg) {
	bool wanted[cards_size];
	bool done = false;
	int *choices = malloc(n * sizeof(int));

	int bufSize = 1024;
	char buffer[bufSize];

	while ( !done ) {
		
		memset(wanted, false, cards_size * sizeof(bool));

		printf("%s\n", msg);
		printf("Or enter 'q' to check other game info.\n");
		interface_printCards(cards, cards_size);

		if ( fgets(buffer, bufSize, stdin) == NULL ) {
			ERROR_PRINT("Input error.\n");
		}

		if ( strcmp(buffer, "q\n") == 0 ) {
			// TODO: Switch to game menu
			break;
		}

		char **idxs;
		int idxs_size;

		mystrsplit(&idxs, &idxs_size, buffer, NULL);

		done = true;
		// Check if input is valid
		if ( idxs_size != n ) {
			WARNING_PRINT("Please enter %d integer(s).\n", n);
			done = false;
		}
		for ( int i=0; i<idxs_size; i++ ) {
			int idx;
			char *endptr;
			idx = strtol(idxs[i], &endptr, 10);
			if ( *endptr != '\0' ) {
				WARNING_PRINT("Please enter integer(s) only.\n");
				done = false;
			} else if ( idx <= 0 || idx > cards_size ) {
				WARNING_PRINT("Please enter integer(s) between [1, %d].\n", cards_size);
				done = false;
			} else if ( wanted[idx-1] ) {
				WARNING_PRINT("Choices duplicated.\n");
				done = false;
			}
			wanted[idx-1] = true;
		}

		for ( int i=0; i<idxs_size; i++ ) {
			free(idxs[i]);
		}
		free(idxs);

		if ( !done ) continue;

		int counter = 0;
		printf("Your choice(s) is:\n");
		for ( int i=0; i<cards_size; i++ ) {
			if ( wanted[i] ) {
				printf("	%d. %s\n", counter+1, cards[i]->name);
				choices[counter++] = i;
			}
		}
		if ( interface_yesOrNo() ) {
			free(msg);
			return choices;
		} else {
			done = false;
		}
	}
	ERROR_PRINT("Unknown error.\n");
	return NULL;
}

int *interface_chooseTake(Player *this, Game *game, Card **cards, int cards_size, int n) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);
	snprintf(buffer, bufSize, "Please choose %d cards from following list.", n);
	return interface_choose(this, game, cards, cards_size, n, buffer);
}

int *interface_chooseDrop(Player *this, Game *game, Card **cards, int cards_size, int n) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);
	snprintf(buffer, bufSize, "Please choose %d card(s) to drop.", n);
	return interface_choose(this, game, cards, cards_size, n, buffer);
}

int interface_selectUse(Player *this, Game *game, Card **cards, int cards_size) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);
	snprintf(buffer, bufSize, "Please choose a card to use");
	return interface_choose(this, game, cards, cards_size, 1, buffer)[0];
}

int interface_selectReact(Player *this, Game *game, Card **cards, int cards_size) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);
	snprintf(buffer, bufSize, "Please choose a card to react");
	return interface_choose(this, game, cards, cards_size, 1, buffer)[0];
}

bool interface_useAbility(Player *this, Game *game) {
	printf("Do you want to use your characteristic ability?\n");
	return interface_yesOrNo();
}
