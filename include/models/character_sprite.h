#ifndef CHARACTER_SPRITE_H
#define CHARACTER_SPRITE_H

#include "character.h"

typedef struct {
    Character *character;          // Pointer to the Character instance
    const unsigned long *bitmap;   // Pointer to the character bitmap
    int width;                     // Sprite width
    int height;                    // Sprite height
    int pos_x;                     // X Position on screen
    int pos_y;                     // Y Position on screen
} CharacterSprite;

extern CharacterSprite sprites[MAX_PROTAGONISTS];  // <-- Just declaration

void draw_character_sprite(CharacterSprite *sprite);

#endif
