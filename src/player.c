#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "player.h"
#include "game.h"
#include "avatar.h"
#include "card.h"
#include "interface.h"
#include "computer_player.h"
#include "debug.h"

Player *Player_init() {
	Player *new = malloc(sizeof(Player));
	new->isComputer = false; // TODO: true
	new->avatar = NULL; // init in game_init
	char *username;
	static int i = 0;
	if(!new->isComputer) {
		printf("---Player %d---\n", ++i);
		username = interface_askName();
	}else {
		printf("Player %d automatically generated...\n", ++i);
		username = computer_askName();
	}
	new->username = malloc(strlen(username)+1);
	strcpy(new->username, username);
	return new;
}

void Player_free(Player *this) {
	// NOTE: Avatars does't free here
	free(this->username);
	free(this);
	DEBUG_PRINT("Done Player_free!\n");
}


int *Player_chooseTake(Player *this, Game *game, Card **cards, int cards_size, int n) {
	if ( !this->isComputer ) {
		return interface_chooseTake(this, game, cards, cards_size, n);
	} else {
		return computer_chooseTake(this, game, cards, cards_size, n);
	}
}

int *Player_chooseDrop(Player *this, Game *game, Card **cards, int cards_size, int n) {
	if ( !this->isComputer ) {
		return interface_chooseDrop(this, game, cards, cards_size, n);
	} else {
		return computer_chooseDrop(this, game, cards, cards_size, n);
	}
}

int Player_selectUse(Player *this, Game *game, Card **cards, int cards_size, Player **target) {
	if ( !this->isComputer ) {
		return interface_selectUse(this, game, cards, cards_size, target);
	} else {
		return computer_selectUse(this, game, cards, cards_size, target);
	}
}

int Player_selectReact(Player *this, Game *game, Card **cards, int cards_size) {
	if ( !this->isComputer ) {
		return interface_selectReact(this, game, cards, cards_size);
	} else {
		return computer_selectReact(this, game, cards, cards_size);
	}
}

bool Player_useAbility(Player *this, Game *game) {
	if ( !this->isComputer ) {
		return interface_useAbility(this, game);
	} else {
		return computer_useAbility(this, game);
	}
}
