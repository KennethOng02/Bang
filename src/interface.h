#pragma once

#include <stdbool.h>
#include <ncurses.h>

#include "player.h"
#include "game.h"
#include "card.h"

WINDOW *boardWin;
WINDOW *inputWin;
WINDOW *messgWin;

void msgQue_push(char *str);

void interface_init();

void moveCurDown(WINDOW *win);

void interface_printCenter(WINDOW *win, int start_row, char *str);

char *interface_askName();

void interface_welcome();

bool interface_yesOrNo(WINDOW *win);

int interface_choose(Player *this, Game *game, Card **cards, bool *validCards, int cards_size, char *msg, bool canPass, bool canBack, bool canUseAbility);

int *interface_chooseTake(Player *this, Game *game, Card **cards, int cards_size, int n);
// select n cards from cards[0..cards_size) to take
// return their indexes

int *interface_chooseDrop(Player *this, Game *game, Card **cards, int cards_size, int n, bool notChoose);
// select n cards from cards[0..cards_size) to drop
// return their indexes

int interface_selectUse(Player *this, Game *game, Card **cards, bool *validCards, int cards_size, bool canUseAbility);
// select 1 card from cards[0..cards_size) to use
// return its index
int interface_selectTarget(Player *this, Game *game, bool *validTargets);

int interface_selectReact(Player *this, Game *game, Card **cards, bool *validReact, int cards_size, bool canUseAbility);
// select 1 card from cards[0..cards_size) to use to react

bool interface_useAbility(Player *this, Game *game);
// whether of not use character ability

char *interface_getCardSuit(int num);

void interface_printTargets(Player *this, Game *game, bool *validTargets, int selected, int l, int r);
void interface_printCards(WINDOW *win, Card **cards, bool *validCards, int selected, int, int);

char *print_role(Role role);

void interface_drawMessg();
void interface_drawInput(Avatar *avatar, bool, bool, bool, bool);
void interface_drawCardVertical(WINDOW *win, int num, int start_row);
void interface_drawCardHorizontal(WINDOW *win, int num, int start_col);
void interface_printPlayerInfoHorizontal(WINDOW *win, Avatar *avatar, int y);
void interface_printPlayerInfoVertical(WINDOW *win, Avatar *avatar, int x);
char *interface_getPlayerEquipment(Avatar *avatar);
void interface_drawBoard(char *username, Game *game);
void interface_draw(char *username, Game *game);
void interface_refresh(char *username, Game *game);
