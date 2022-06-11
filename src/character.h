#pragma once

typedef struct character Character;
struct character {
	char *name;
	int hp;
	char *intro;
};

Character *Character_init(const char *name, const int hp, const char *intro);

void Character_free(Character *this);

Character **buildCharacterDeck();

Character **genCharacterDeck(int deck_size);

void printCharacterDeck(Character ** deck, int deck_size);
