#ifndef SKILL_H
#define SKILL_H

#include "constants.h"
// #include "character.h"

#define SKILL_KEY 't'
#define MAX_SKILLS 2

typedef struct {
    char name[MAX_NAME_LENGTH];
    int is_aoe;         // 0 = single target, 1 = AOE
    int base_damage;
} Skill;

typedef enum {
NO_ACTION,
ATTACK_SINGLE,
ATTACK_ALL
} SkillActionType;

typedef struct {
SkillActionType action;
int selected_enemy; // Only used if action == ATTACK_SINGLE
} SkillSelectionResult;

SkillSelectionResult handle_skill_selection();
void apply_skill_effect(SkillSelectionResult result);

#endif
