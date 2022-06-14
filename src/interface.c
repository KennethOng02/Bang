#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "interface.h"
#include "player.h"
#include "game.h"
#include "card.h"
#include "debug.h"
#include "mylib.h"
#include "ANSI-color-codes.h"

// 救命，幫我改介面跟英文=w=
// 我沒有美術天分，英文又很破...

char *interface_askName() {
	char *name = calloc(32, sizeof(char));
	while(1) {
		printf("Please input your name: ");

		if(fgets(name, 32, stdin) == NULL) {
			clearerr(stdin);
			printf("\n");
			continue;
		}
		if ( name[strlen(name)-1] != '\n' ) {
			WARNING_PRINT("Username too long, at most 31 characters.\n");
			char ch = 0;
			while ( ch != '\n' ) {
				scanf("%c", &ch);
			}
			continue;
		}

		while(isspace((unsigned char)*name)) name++;
		
		char *end = name + strlen(name) - 1;
		while(end > name && isspace((unsigned char)*end)) end--;
		end[1] = '\0';

		if(*name != 0) {
			break;
		}
		WARNING_PRINT("Empty Username.\n");
	}
	printf("|%s|\n", name);
	return name;
}

void interface_welcome() {
	printf(CYN"____________________   __________\n"
			   "___  __ )__    |__  | / /_  ____/\n"
			   "__  __  |_  /| |_   |/ /_  / __  \n"
			   "_  /_/ /_  ___ |  /|  / / /_/ /  \n"
			   "/_____/ /_/  |_/_/ |_/  \\____/   \n"
				 MAG"COPYRIGHT to Din, Brian, Kenneth\n"reset);
	return;
}

void interface_printCards(Card **cards, int cards_size) {
	for ( int i=0; i<cards_size; i++ ) {
		printf("%d) %s\n", i+1, cards[i]->name);
	}
}

bool interface_yesOrNo(void) {
	int bufSize = 10;
	char buffer[bufSize];
	while ( true ) {
		printf("(%sy%s/%sn%s):", GRN, reset, RED, reset);
		if ( fgets(buffer, bufSize, stdin) == NULL ) {
			WARNING_PRINT("Please enter '%sy%s' or '%sn%s'.\n", GRN, reset, RED, reset);
			clearerr(stdin);
			continue;
		}
		if ( strcmp(buffer, "y\n") == 0 ) return true;
		if ( strcmp(buffer, "n\n") == 0 ) return false;
	}
	return false;
}

int *interface_choose(Player *this, Game *game, Card **cards, int cards_size, int n, char *msg, bool notChoose) {
	bool wanted[cards_size];
	bool done = false;
	int *choices = malloc(n * sizeof(int));

	int bufSize = 1024;
	char buffer[bufSize];

	while ( !done ) {
		
		memset(wanted, false, cards_size * sizeof(bool));

		printf("%s\n", msg);
		if ( notChoose ) printf("You can enter '%s0%s' if you don't want/have card to use.\n", GRN, reset);
		printf("Or enter '%si%s' to check other game info.\n", GRN, reset);
		printf("Or enter '%sq%s' to check other game info.\n", GRN, reset);
		interface_printCards(cards, cards_size);

		if ( fgets(buffer, bufSize, stdin) == NULL ) {
			clearerr(stdin);
			continue;
		}

		if ( strcmp(buffer, "i\n") == 0 ) {
			// TODO: Switch to game menu
			DEBUG_PRINT("This feature is not supported in your region.\n");
			interface_menu(game);
			continue;
		}

		if ( strcmp(buffer, "q\n") == 0 ) {
			if(interface_yesOrNo()) {
				Game_exit(game);
			}
			continue;
		}

		if ( notChoose && strcmp(buffer, "0\n") == 0 ) {
			free(choices);
			return NULL;
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
	return interface_choose(this, game, cards, cards_size, n, buffer, false);
}

int *interface_chooseDrop(Player *this, Game *game, Card **cards, int cards_size, int n) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);
	snprintf(buffer, bufSize, "Please choose %d card(s) to drop.", n);
	return interface_choose(this, game, cards, cards_size, n, buffer, false);
}

int interface_selectUse(Player *this, Game *game, Card **cards, int cards_size) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);
	snprintf(buffer, bufSize, "Please choose a card to use.");
	int *ret = interface_choose(this, game, cards, cards_size, 1, buffer, true);
	if ( ret != NULL ) {
		return ret[0];
	}
	return -1;
}

int interface_selectReact(Player *this, Game *game, Card **cards, int cards_size) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);
	snprintf(buffer, bufSize, "Please choose a card to respond.");
	int *ret = interface_choose(this, game, cards, cards_size, 1, buffer, true);
	if ( ret != NULL ) {
		return ret[0];
	}
	return -1;
}

bool interface_useAbility(Player *this, Game *game) {
	printf("Do you want to use your characteristic ability?\n");
	return interface_yesOrNo();
}

void interface_menu(Game *game) {
	interface_playerInfo(game->avatars);
	/* interface_discardedPileInfo(game->avatars); */
	return;
}

void interface_playerInfo(Avatar **avatars) {
	printf(MAG"---Player Info---\n"reset);	
	for(int i = 0; i < 4; i++) {
		printf("Player %d\n", ++i);
		if(!avatars[i]) {
			printf("DEAD\n");
		}
		if(avatars[i]->player->isComputer) {
		}
	}
	return;
}
