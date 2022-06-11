#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "mylib.h"
#include "character.h"

Character *Character_init(const char *name, const int hp, const char *intro) {
	if(name == NULL || intro == NULL)
		ERROR_PRINT("NULL pointer Mother Fucker !!\n");

	if(hp < 0 && hp > 4)
		ERROR_PRINT("hp must be between 0 and 4");

	Character *new = malloc(sizeof(Character));

	new->name = malloc(strlen(name) + 1);
	strcpy(new->name, name);
	new->hp = hp;
	new->intro = malloc(strlen(intro)+1);
	strcpy(new->intro, intro);
	return new;
}

void Character_free(Character *this) {
	free(this->name);
	free(this->intro);
	free(this);
}

Character **buildCharacterDeck() {
	FILE *pfile = fopen("src/character.txt", "r");
	assert(pfile);

	Character **character = calloc(16, sizeof(Character *));

	char *buffer = calloc(1024, sizeof(char));
	int i = 0;
	while(fgets(buffer, 1024, pfile) != NULL) {
		char **line;
		int counter;
		mystrsplit(&line, &counter, buffer, "\"");
		character[i++] = Character_init(line[1], strtod(line[3], NULL), line[5]);
	}
	return character;
}

Character **genCharacterDeck(int deck_size) {
	Character **deck = buildCharacterDeck(); 
	SHUFFLE(deck, deck_size, Character *);
	return deck;
}

void printCharacterDeck(Character ** deck, int deck_size) {
	for(int i = 0; i < deck_size; i++) {
		printf("Character %d\n", i+1);
		printf("	Name: %s\n", deck[i]->name);
		printf("	hp: %d\n", deck[i]->hp);
		printf("	intro: %s\n", deck[i]->intro);
	}
}
