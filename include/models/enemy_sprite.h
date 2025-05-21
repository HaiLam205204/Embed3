#ifndef ENEMY_SPRITE_H
#define ENEMY_SPRITE_H

#include "enemy.h"
typedef struct {
    EnemyModel *enemy;
    const unsigned long *bitmap;
    int width;
    int height;
    int pos_x;
    int pos_y;
} EnemySprite;

extern EnemySprite enemy_sprites[MAX_ENEMIES];  // <-- Just declaration

void draw_enemy_sprite(EnemySprite *sprite);
void draw_combat_enemies1(int pos_x, int pos_y, const unsigned long *character_bitmap, int image_width, int image_height);
void draw_combat_enemies2(int pos_x, int pos_y, const unsigned long *character_bitmap, int image_width, int image_height);

#endif