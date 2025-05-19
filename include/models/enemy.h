#ifndef ENEMY_H
#define ENEMY_H

#include "skill.h"
#include "constants.h"

#define MAX_ENEMIES 3

typedef struct {
    char name[MAX_NAME_LENGTH];
    int max_hp;
    int current_hp;
    int enemy_type;  // 1 or 2
    Skill skills[MAX_SKILLS_PER_ENEMY];
} Enemy;

extern Enemy enemies[MAX_ENEMIES];

#endif
