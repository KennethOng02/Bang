#pragma once

#include <stdbool.h>
#include <ncurses.h>

#include "player.h"
#include "game.h"
#include "card.h"

WINDOW *boardWin;
WINDOW *inputWin;
WINDOW *messgWin;

void interface_printCenter(WINDOW *win, int start_row, char *str);

char *interface_askName();

void interface_welcome();

int *interface_choose(Player *this, Game *game, Card **cards, int cards_size, int n, char *msg, bool notChoose);
bool interface_yesOrNo();

int *interface_chooseTake(Player *this, Game *game, Card **cards, int cards_size, int n);
// select n cards from cards[0..cards_size) to take
// return their indexes

int *interface_chooseDrop(Player *this, Game *game, Card **cards, int cards_size, int n);
// select n cards from cards[0..cards_size) to drop
// return their indexes

int interface_selectUse(Player *this, Game *game, Card **cards, int cards_size);
// select 1 card from cards[0..cards_size) to use
// return its index
Player *interface_selectTarget(Player *this, Game *game);

int interface_selectReact(Player *this, Game *game, Card **cards, int cards_size);
// select 1 card from cards[0..cards_size) to use to react

bool interface_useAbility(Player *this, Game *game);
// whether of not use character ability


void interface_printCards(Card **cards, int cards_size);

void interface_menu(Game *game, Player *self);

char *print_role(Role role);

void interface_playerInfo(Game *game, Player *self);
void interface_drawMessg();
void interface_drawInput(Avatar *avatar);
void interface_printCardVertical(WINDOW *win, int num, int start_row);
void interface_printCardHorizontal(WINDOW *win, int num, int start_col);
void interface_printPlayerInfoHorizontal(WINDOW *win, Avatar *avatar, int y);
void interface_printPlayerInfoVertical(WINDOW *win, Avatar *avatar, int x);
void interface_drawBoard(char *username, Game *game);
void interface_draw(char *username, Game *game);
void interface_erase();
