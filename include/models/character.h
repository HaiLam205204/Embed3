#ifndef CHARACTER_H
#define CHARACTER_H

#include "action.h"
#include "constants.h"

typedef struct {
    char name[MAX_NAME_LENGTH];
    int is_main_character;
    int current_hp;
    int max_hp;
    Action current_action;
} Character;

#endif
