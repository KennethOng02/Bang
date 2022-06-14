#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "player.h"
#include "game.h"
#include "avatar.h"
#include "card.h"
#include "interface.h"
#include "debug.h"

void Player_initWithoutMalloc(Player **player, bool isComputer, char *username, Avatar *avatar) {
	Player *this = *player;
	this->isComputer = isComputer;
	this->username = malloc(strlen(username)+1);
	strcpy(this->username, username);
	this->avatar = avatar;
	DEBUG_PRINT("Finish initiation of player %s. His avatar: %d\n", username, this->avatar->id);
}

void Player_free(Player *this) {
	// NOTE: Avatars does't free here
	free(this->username);
	free(this);
}


int *Player_chooseTake(Player *this, Game *game, Card **cards, int cards_size, int n) {
	if ( !this->isComputer ) {
		return interface_chooseTake(this, game, cards, cards_size, n);
	} else {
		DEBUG_PRINT("Computer ah north down.\n");
		return NULL;
	}
}

int *Player_chooseDrop(Player *this, Game *game, Card **cards, int cards_size, int n) {
	if ( !this->isComputer ) {
		return interface_chooseDrop(this, game, cards, cards_size, n);
	} else {
		DEBUG_PRINT("Computer ah north down.\n");
		return NULL;
	}
}

int Player_selectUse(Player *this, Game *game, Card **cards, int cards_size) {
	if ( !this->isComputer ) {
		return interface_selectUse(this, game, cards, cards_size);
	} else {
		DEBUG_PRINT("Computer ah north down.\n");
		return -1;
	}
}

int Player_selectReact(Player *this, Game *game, Card **cards, int cards_size) {
	if ( !this->isComputer ) {
		return interface_selectReact(this, game, cards, cards_size);
	} else {
		DEBUG_PRINT("Computer ah north down.\n");
		return -1;
	}
}

bool Player_useAbility(Player *this, Game *game) {
	if ( !this->isComputer ) {
		return interface_useAbility(this, game);
	} else {
		DEBUG_PRINT("Computer ah north down.\n");
		return false;
	}
}
