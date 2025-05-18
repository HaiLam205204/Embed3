#ifndef PERSONA_H
#define PERSONA_H

#include "constants.h"
#include "skill.h"

typedef struct {
    char name[MAX_NAME_LENGTH];
    Skill skills[MAX_SKILLS_PER_PERSONA];
} Persona;

#endif
