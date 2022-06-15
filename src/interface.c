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
		printf("%d) ", i+1);
		if ( cards[i] ) {
			printf("%s\n", cards[i]->name);
		} else {
			printf("UNKNOWN\n");
		}
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
		if ( notChoose ) printf("Enter '%s0%s' to pass.\n", GRN, reset);
		printf("Enter '%si%s' to check other game info.\n", GRN, reset);
		printf("Enter '%sq%s' to quit\n", GRN, reset);
		interface_printCards(cards, cards_size);

		if ( fgets(buffer, bufSize, stdin) == NULL ) {
			clearerr(stdin);
			continue;
		}

		if ( strcmp(buffer, "i\n") == 0 ) {
			// TODO: Switch to game menu
			DEBUG_PRINT("This feature is fully implemented yet.\n");
			interface_menu(game, this);
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

int interface_selectUse(Player *this, Game *game, Card **cards, int cards_size, Player **target) {
	int bufSize = 1024;
	char *buffer = malloc(bufSize);
	snprintf(buffer, bufSize, "Please choose a card to use.");
	int *ret = interface_choose(this, game, cards, cards_size, 1, buffer, true);
	if ( ret == NULL ) {
		return -1;
	}

	if(cards[ret[0]]->type == 0) {
		*target = NULL;
		return ret[0];
	}

	snprintf(buffer, bufSize, "Please choose which player as target.");
	for(int i = 0; i < game->numAvatar; i++) {
		printf("%d) Player %d (%s) %s\n", i, game->avatars[i]->id, game->avatars[i]->player->username, game->avatars[i]->role == SHERIFF ? "(SHERIFF)" : "");
	}
	int choice;
	while(1) {
		printf("Choice: ");
		scanf("%d", &choice);
		if(choice < 0 && choice > game->numAvatar) {
			printf("Please input within range 0 to %d\n", game->numAvatar);
			continue;
		}
		break;
	}
	*target = game->avatars[choice]->player;
	return ret[0];
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

void interface_menu(Game *game, Player *self) {
	interface_playerInfo(game, self);
	/* interface_discardedPileInfo(game->avatars); */
	return;
}

char *print_role(Role role) {
	if(role == SHERIFF) {
		return "SHERIFF";
	}
	if(role == DEPUTY) {
		return "DEPUTY";
	}
	if(role == OUTLAW) {
		return "OUTLAW";
	}
	if(role == RENEGADE) {
		return "RENEGADE";
	}
	if(role == ROLE_MAX) {
		return "ROLE_MAX";
	}
	return "NULL";
}

void interface_playerInfo(Game *gametmp, Player* self) {
	Game *game = Game_queryInfo(self);
	Avatar **avatars_list = game->avatars;
	printf("%d\n", game->numAvatar);
	printf(MAG"---Player Info---\n"reset);	
	for(int i = 0; i < game->numAvatar; i++) {
		printf("Player %d\n", avatars_list[i]->id);
		printf("	Username: %s%s%s\n", GRN, avatars_list[i]->player->username, reset);
		if(avatars_list[i]->isDead) {
			printf(RED"	---DEAD---\n"reset);
			printf("	Role: %s%s%s\n", MAG, print_role(avatars_list[i]->role), reset);
			printf("	Character: %s%s%s\n", MAG, avatars_list[i]->character->name, reset);
		}else {

			printf("	Hp: %d\n", avatars_list[i]->hp);

			printf("	Role: " );
			if ( avatars_list[i]->role != UNKNOWN ) {
				printf( MAG"%s"reset"\n", print_role(avatars_list[i]->role));
			} else {
				printf("UNKNOWN\n");
			}

			printf("	Character: %s%s%s\n", MAG, avatars_list[i]->character->name, reset);

			printf("	Card Num: %d\n", avatars_list[i]->cards_size);

			printf("	Equipment: \n");
			Equipment *eqi = avatars_list[i]->equipment;
			if( eqi->gun != NULL) printf("		Gun: %s\n", eqi->gun->name);
			if( eqi->armour != NULL) printf("		Armour: %s\n", eqi->armour->name);
			if( eqi->horseMinus != NULL) printf("		Horse: %s\n", eqi->horseMinus->name);
			if( eqi->horsePlus != NULL) printf("		Horse: %s\n",eqi->horsePlus->name);
			if( eqi->bomb != NULL) printf("		%s\n", eqi->bomb->name);
			if( eqi->jail != NULL) printf("		%s\n", eqi->jail->name);
		}
	}
	Game_freeCopy(game);
	return;
}
