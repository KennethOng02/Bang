#pragma once
#include "card.h"
#define CARD_START 0
#define CARD_HAND_START 1
#define CARD_BANG 2
#define CARD_MISS 3
#define CARD_GATLING 4
#define CARD_INDIANS 5
#define CARD_PANIC 6
#define CARD_BALOU 7
#define CARD_STAGECOACH 8
#define CARD_FARGO 9
#define CARD_STORE 10
#define CARD_BEER 11
#define CARD_SALOON 12
#define CARD_DUEL 13
#define CARD_HAND_END 14
#define CARD_ARMOUR_START 15
#define CARD_BARREL 16
#define CARD_SCOPE 17
#define CARD_MUSTANG 18
#define CARD_ARMOUR_END 19
#define CARD_GUN_START 20
#define CARD_VOLCANIC 21
#define CARD_SCHOFIELD 22
#define CARD_REMINGTON 23
#define CARD_CARABINE 24
#define CARD_WINCHEDTER 25
#define CARD_GUN_END 26
#define CARD_JUDGE_START 27
#define CARD_JAIL 28
#define CARD_DYNAMITE 29
#define CARD_JUDGE_END 30
#define CARD_END 31
static int (*Card_funArr[])(Avatar *,Avatar *, Game *, Card *) = { NULL, NULL, &play_CARD_BANG, &play_CARD_MISS, &play_CARD_GATLING, &play_CARD_INDIANS, &play_CARD_PANIC, &play_CARD_BALOU, &play_CARD_STAGECOACH, &play_CARD_FARGO, &play_CARD_STORE, &play_CARD_BEER, &play_CARD_SALOON, &play_CARD_DUEL, NULL, NULL, &play_CARD_BARREL, &play_CARD_SCOPE, &play_CARD_MUSTANG, NULL, NULL, &play_CARD_VOLCANIC, &play_CARD_SCHOFIELD, &play_CARD_REMINGTON, &play_CARD_CARABINE, &play_CARD_WINCHEDTER, NULL, NULL, &play_CARD_JAIL, &play_CARD_DYNAMITE, NULL, NULL, };