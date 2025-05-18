#ifndef SKILL_H
#define SKILL_H

#include "constants.h"

typedef struct {
    char name[MAX_NAME_LENGTH];
    int is_aoe;         // 0 = single target, 1 = AOE
    int base_damage;
} Skill;

#endif
