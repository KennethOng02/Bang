#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "player.h"
#include "game.h"
#include "avatar.h"
#include "card.h"
#include "interface.h"
#include "computer_player.h"
#include "print.h"

Player *Player_init(int id, Avatar *avatar) {
	Player *new = malloc(sizeof(Player));

	new->id = id;

	/* printf("Player %d is computer?\n", new->id); */
	
	new->avatar = avatar;

	/* new->isComputer = interface_yesOrNo(); */
	new->isComputer = id == 1 ? 0: 1;

	char *username;
	if(!new->isComputer) {
		/* printf("---Player %d---\n", new->id); */
		username = interface_askName();
	}else {
		/* printf("Player %d automatically generated...\n", new->id); */
		username = computer_askName();
	}
	new->username = malloc(strlen(username)+1);
	strcpy(new->username, username);
	return new;
}

void Player_free(Player *this) {
	free(this->username);
	free(this);
	DEBUG_PRINT("Done Player_free!\n");
}


int *Player_chooseTake(Player *this, Game *game, Card **cards, int cards_size, int n, bool undo) {
	if ( !this->isComputer ) {
		return interface_chooseTake(this, game, cards, cards_size, n, undo);
	} else {
		return computer_chooseTake(this, game, cards, cards_size, n);
	}
}

int *Player_chooseDrop(Player *this, Game *game, Card **cards, int cards_size, int n, bool notChoose) {
	if ( !this->isComputer ) {
		return interface_chooseDrop(this, game, cards, cards_size, n, notChoose);
	} else {
		return computer_chooseDrop(this, game, cards, cards_size, n);
	}
}

int Player_selectUse(Player *this, Game *game, Card **cards, bool *validCards, int cards_size, bool canUseAbility) {
	if ( !this->isComputer ) {
		return interface_selectUse(this, game, cards, validCards, cards_size, canUseAbility);
	} else {
		return computer_selectUse(this, game, cards, validCards, cards_size);
	}
}

int Player_selectTarget(Player *this, Game *game, bool *validTargets) {
	if ( !this->isComputer ) {
		return interface_selectTarget(this, game, validTargets);
	} else {
		return computer_selectTarget(this, game, validTargets);
	}
}

int Player_selectReact(Player *this, Game *game, Card **cards, bool *validReact, int cards_size, bool canUseAbility) {
	if ( !this->isComputer ) {
		return interface_selectReact(this, game, cards, validReact, cards_size, canUseAbility);
	} else {
		return computer_selectReact(this, game, cards, validReact, cards_size);
	}
}

bool Player_useAbility(Player *this, Game *game) {
	if ( !this->isComputer ) {
		return interface_useAbility(this, game);
	} else {
		return computer_useAbility(this, game);
	}
}
