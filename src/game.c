#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "debug.h"
#include "mylib.h"
#include "avatar.h"
#include "deck.h"
#include "card.h"
#include "game.h"
#include "player.h"
#include "interface.h"
#include "cardid.h"
#include "characterid.h"

static Game * game;

void Game_init(int numAvatar) {
	interface_welcome();

	game = malloc(sizeof(Game));
	
	// initiate Card *deck[DECK_SIZE]
	game->deck = Deck_gen(DECK_SIZE);

	// initiate Card *discardPile[DECK_SIZE] = {NULL}
	game->discardPile = calloc(DECK_SIZE, sizeof(Card *));
	for(int i = 0; i < DECK_SIZE; i++) {
		game->discardPile = Deck_init(DECK_SIZE);
	}

	Character **character_deck = Deck_genCharacter(CHARACTER_SIZE);
	// initiate Avatar *avatars[numAvatar]
	game->numAvatar = numAvatar;
	game->numAvailableAvatar = numAvatar;
	Role *roles = genRoles(numAvatar);
	game->avatars = malloc(game->numAvatar * sizeof(Avatar *));
	for(int i = 0; i < numAvatar; i++ ) {
		// Initiate player and avatar
		game->avatars[i] = Avatar_init(i+1, character_deck[i], roles[i]);
		for ( int _=0; _<game->avatars[i]->hp; _++ ) {
			Avatar_draw(game->avatars[i], game);
		}
	}

	DEBUG_PRINT("Done Game_init\n");
	
	for(int i = 0; i < CHARACTER_SIZE; i++)
		Character_free(character_deck[i]);
	free(character_deck);
	free(roles);
}

Game *Game_copy(Game *this) {
	Game *copy = malloc(sizeof(Game));
	copy->deck = Deck_copy(this->deck);
	copy->discardPile = Deck_copy(this->discardPile);
	copy->numAvatar = this->numAvatar;
	copy->numAvailableAvatar = this->numAvailableAvatar;
	copy->avatars = malloc(this->numAvatar * sizeof(Avatar *));
	for ( int i=0; i<this->numAvatar; i++ ) {
		copy->avatars[i] = Avatar_copy(this->avatars[i]);
	}
	return copy;
}

void Game_free() {
	for ( int i=0; i<game->numAvatar; i++ ) {
		Avatar_free(game->avatars[i]);
	}
	free(game->avatars);

	Deck_free(game->deck, DECK_SIZE);

	Deck_free(game->discardPile, DECK_SIZE);

	free(game);
	
	DEBUG_PRINT("Game_free Done !\n");
	return;
}

void Game_freeCopy(Game *this) {
	for ( int i=0; i<this->numAvatar; i++ ) {
		Avatar_freeCopy(this->avatars[i]);
	}
	free(this->avatars);
	Deck_free(this->deck, DECK_SIZE);
	Deck_free(this->discardPile, DECK_SIZE);
	free(this);
	return;
}

void Game_run() {
	Avatar *sheriff = NULL;
	for ( int i=0; i<game->numAvatar; i++ ) {
		if ( game->avatars[i]->role == SHERIFF ) {
			sheriff = game->avatars[i];
			break;
		}
	}

	if ( sheriff == NULL ) {
		ERROR_PRINT("No SHERIFF in game game.\n");
	}
	Avatar *curAvatar = sheriff;

	DEBUG_PRINT("Stating game loop\n");
	while ( 1 ) {
		Avatar_onTurn(curAvatar, game);
		DEBUG_PRINT("Avatar %d's turn finish.\n", curAvatar->id);
		curAvatar = Game_nextAvailableAvatar(curAvatar);
	}
}

void Game_exit() {
	puts(GRN"---Exit BANG---"reset);
	Game_free();
	exit(0);
	return;
}

int Game_findIndex(Avatar *avatar)	{
	for ( int i=0; i<game->numAvatar; i++ ) {
		if ( game->avatars[i]->id == avatar->id ) {
			return i;
		}
	}
	return -1;
}

Avatar *Game_nextAvailableAvatar(Avatar *avatar) {
	int idx = Game_findIndex(avatar);
	if ( idx == -1 ) {
		ERROR_PRINT("Avatar %d not in game.\n", avatar->id);
	}
	do {
		idx = (idx+1) % game->numAvatar;
	} while ( game->avatars[idx]->isDead );
	return game->avatars[idx];
}

Avatar *Game_firstAvailableAvatar() {
	for ( int i=0; i<game->numAvatar; i++ ) {
		if ( !game->avatars[i]->isDead ) {
			return game->avatars[i];
		}
	}
	ERROR_PRINT("Everyone dead.\n");
	return NULL;
}

void Game_reShuffle() {
	SWAP(game->discardPile, game->deck, Deck *);
	SHUFFLE(game->deck->card_pile, game->deck->top+1, Card *);
}

void Game_checkWin() {
	Avatar *sheriff = NULL;
	bool teamSheriff = false;
	bool teamOutlaw = false;
	bool teamRenegade = false;
	for ( int i=0; i<game->numAvatar; i++ ) {
		if ( game->avatars[i]->role == SHERIFF ) {
			sheriff = game->avatars[i];
		}
		if ( !game->avatars[i]->isDead ) {
			switch ( game->avatars[i]->role ) {
			case SHERIFF:
			case DEPUTY:
				teamSheriff = true;
				break;
			case OUTLAW:
				teamOutlaw = true;
				break;
			case RENEGADE:
				teamRenegade = true;
				break;
			default:
				ERROR_PRINT("Unknown role.\n");
			}
		}
	}
	if ( sheriff->isDead ) {
		if ( teamOutlaw ) {
			printf("Outlaws win!\n");
			Game_exit();
		} else if ( teamRenegade && !teamSheriff ) {
			printf("Renegade wins!\n");
			Game_exit();
		}
	}
	if ( !teamOutlaw && !teamRenegade ) {
		printf("Sheriff wins!\n");
		Game_exit();
	}
}

Game *Game_queryInfo( Player *player ) {
	Game *copy = Game_copy(game);
	for ( int i=0; i<copy->deck->top+1; i++ ) {
		copy->deck->card_pile[i] = NULL;
	}
	for ( int i=0; i<copy->numAvatar; i++ ) {
		if ( copy->avatars[i]->isDead ) {
			continue;
		}
		if ( copy->avatars[i]->id != player->id ) {
			for ( int j=0; j<copy->avatars[i]->cards_size; j++ ) {
				copy->avatars[i]->cards[j] = NULL;
			}
			if ( copy->avatars[i]->role != SHERIFF ) {
				copy->avatars[i]->role = UNKNOWN;
			}
		}
	}
	return copy;
}

bool validPlay(Avatar *user, Avatar *target, Card *card) {

	if ( target && target->isDead ) {
		//WARNING_PRINT("Cannot play %s on dead player.\n", card->name);
		return false;
	}
	if ( card->type != CARD_DIST_NON ) {
		if ( target == NULL ) {
			//WARNING_PRINT("No target when playing %s.\n", card->name);
			return false;
		} else if ( target->id == user->id ) {
			//WARNING_PRINT("Cannot play %s on self.\n", card->name);
			return false;
		} else if ( card->type == CARD_DIST_ONE && Avatar_calcDist(game, user, target) > 1 ) {
			//WARNING_PRINT("Cannot play %s on %s: Too far.\n", card->name, user->player->username);
			return false;
		} else if ( card->type == CARD_DIST_VISION && Avatar_calcDist(game, user, target) > Avatar_calcVision(user) ) {
			//WARNING_PRINT("Cannot play %s on %s: Too far.\n", card->name, user->player->username);
			return false;
		}
	}

	switch(card->id) {
		
	case CARD_BANG:
		return true;
		break;
		
	case CARD_MISS:
		if ( user->character->id == Calamity_Janet ) {
			return Avatar_calcDist(game, user, target) <= Avatar_calcVision(user);
		} else {
			//WARNING_PRINT("Cannot play MISS in your turn.\n");
			return false;
		}
		
	case CARD_GATLING:
	case CARD_INDIANS:
		return true;

	case CARD_PANIC:
	case CARD_BALOU:
		if ( target->cards_size == 0 ) {
			bool hasEquip = false;
			for ( Card ** iter = (Card **)target->equipment; iter < (Card **)(target->equipment+1); iter++) {
				if ( *iter ) hasEquip = true;
			}
			if(hasEquip == false) {
				//printf("the target you choose had no card left!\n");
			}
			return hasEquip;
		}
		return true;

	case CARD_STAGECOACH:
	case CARD_FARGO:
	case CARD_STORE:
		return true;

	case CARD_BEER:
		if( game->numAvailableAvatar <= 2) {
			//WARNING_PRINT("You can't use beer when only two player left!\n");
			return false;
		}
		if ( user->hp == user->hp_max ) {
			//WARNING_PRINT("Your hp is max.\n");
			return false;
		}
		return true;

	case CARD_SALOON: {
		bool valid = false;
		for ( int i=0; i<game->numAvatar; i++ ) {
			if ( !game->avatars[i]->isDead && game->avatars[i]->hp != game->avatars[i]->hp_max ) {
				valid = true;
			}
		}
		//if ( !valid ) WARNING_PRINT("Everyone has max hp.\n");
		return valid;
	}
	
	case CARD_DUEL:
		return true;

	case CARD_BARREL:
	case CARD_APPALOOSA:
	case CARD_MUSTANG:
	case CARD_VOLCANIC:
	case CARD_SCHOFIELD:
	case CARD_REMINGTON:
	case CARD_CARABINE:
	case CARD_WINCHEDTER:
		return true;

	case CARD_JAIL:
		if ( target->role == SHERIFF ) {
			//WARNING_PRINT("You can not use jail to sheriff!\n");
			return false;
		}
		//if ( target->equipment->jail != NULL ) WARNING_PRINT("He has jail already.\n");
		return target->equipment->jail == NULL;

	case CARD_DYNAMITE:
		//if ( user->equipment->bomb != NULL ) WARNING_PRINT("You has dynamite already.\n");
		return user->equipment->bomb == NULL;
	
	default:
		ERROR_PRINT("Unknown cara.\n");
	}
	return false;
}
