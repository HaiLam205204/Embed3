#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "models/enemy.h"
#include "models/enemy_sprite.h"
#include "combat_character.h"

extern EnemyModel enemy[MAX_ENEMIES];
// extern EnemySprite enemy_sprites[MAX_ENEMIES];
extern int num_enemies;

void deal_damage(int index, int amount);
void remove_enemy(int index);
void recalculate_enemy_sprite_positions();


#endif