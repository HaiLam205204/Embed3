#ifndef ACTION_H
#define ACTION_H

#include "skill.h"
#include "item.h"
#include "persona.h"

typedef enum {
    ACTION_NONE,
    ACTION_ATTACK,
    ACTION_ITEM,
    ACTION_PERSONA,
    ACTION_SKILL,
    ACTION_RUN
} ActionType;

typedef struct {
    ActionType type;
    union {
        Skill skill;
        Item item;
        Persona persona;
        int run_flag;  
    } action_data;

    int skill_index;    // only if applicable
    int target_enemy;   // index of the target enemy (0-based)
} Action;

#endif
