#ifndef ENEMY_H
#define ENEMY_H

#include "skill.h"
#include "constants.h"

typedef struct {
    char name[MAX_NAME_LENGTH];
    int max_hp;
    int current_hp;
    Skill skills[MAX_SKILLS_PER_ENEMY];
} Enemy;

#endif
