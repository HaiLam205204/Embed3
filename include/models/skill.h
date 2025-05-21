#ifndef SKILL_H
#define SKILL_H

#include "constants.h"
// #include "character.h"

#define SKILL_KEY 't'
#define MAX_SKILLS 2

// enum SkillType {
//     SKILL_SINGLE_TARGET,
//     SKILL_ALL_ENEMIES
// };

// typedef struct {
//     char name[MAX_NAME_LENGTH];
//     enum SkillType type;
//     int damage;
// } Skill;

// Skill skills[MAX_SKILLS] = {
//     {"Strike", SKILL_SINGLE_TARGET, 25},
//     {"AoE Blast", SKILL_ALL_ENEMIES, 15}
// };

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
// void draw_skill_option_terminal(int skill_index);
// void use_skill(Character* user, Skill skill);

#endif
