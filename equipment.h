#pragma once

#include "card.h"

typedef struct equipment Equipment;
struct equipment {
	Card *gun;
	Card *armour;
	Card *horsePlus;
	Card *horseMinus;
	Card *Jail;
	Card *bomb;
};

Equipment * Equipment_init() {
	Equipment * new = malloc(sizeof(Equipment));
	new -> gun = NULL;
	new -> armour = NULL;
	new -> horsePlus = NULL;
	new -> horseMinus = NULL;
	new -> Jail = NULL;
	new -> bomb = NULL;
	return new;
}
