#ifndef CHARACTER_H
#define CHARACTER_H

#include "action.h"
#include "constants.h"

#define MAX_PROTAGONISTS 4

typedef enum {
    PERSONA_ORPHEUS = 0,
    PERSONA_PIXIE = 1
} PersonaType;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int is_main_character;
    int current_hp;
    int max_hp;
    int has_acted;  // 0 = not acted, 1 = acted this turn
    int healing_item_quantity;  // NEW: Number of healing items
    Action current_action;
    PersonaType current_persona; // New field
} Character;

extern Character protagonists[MAX_PROTAGONISTS];

#endif
