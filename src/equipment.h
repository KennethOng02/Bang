#pragma once

typedef struct equipment Equipment;
#include "card.h"

struct equipment {
	Card *gun;
	Card *armour;
	Card *horsePlus;
	Card *horseMinus;
	Card *jail;
	Card *bomb;
};

Equipment * Equipment_init();

void Equipment_free(Equipment *this);
