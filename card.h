#pragma once

typedef struct card Card;
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "player.h"

#define CARDTYPE_HAND 1
#define CARDTYPE_EQUIPMENT 2

struct card {
	char *name;
	int type; // CARDTYPE_HAND or CARDTYPE_EQUIPMENT
	int suit; // [0 - 51] //[Spade, Heart, Diamond, Club]
	// int (* play)(Player * user, Player ** target);
};

Card *Card_init(const char *name, const int type, const int suit) 
{
	if ( name == NULL ) {
		ERROR_PRINT("it's NULL pointer Mother Fucker !!\n");
	}
	if ( type != CARDTYPE_HAND && type != CARDTYPE_EQUIPMENT ) {
		ERROR_PRINT("type must within [1, 2]\n");
	}
	if ( suit < 0 && suit >= 52 ) {
		ERROR_PRINT("suit must within [0, 51]\n");
	}
	Card *new = malloc(sizeof(Card));
	new->name = malloc(strlen(name)+1);
	strcpy(new->name, name);
	new->type = type;
	new->suit = suit;
	// new->play = NULL;
	return new;
}

//int Card_playBang(Player * user, Player ** target);
//int Card_playXXX(Player * user, Player ** target);
