#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "models/enemy.h"
#include "models/character.h"
#include "models/enemy_sprite.h"
#include "combat_character.h"

extern EnemyModel enemy[MAX_ENEMIES];
// extern EnemySprite enemy_sprites[MAX_ENEMIES];
extern int num_enemies;
extern int num_protagonists;

void deal_damage(int index, int amount);
void remove_enemy(int index);
void remove_protagonist(int index);
void recalculate_enemy_sprite_positions();
void enemy_turn(Character *protagonists, int num_protagonists);
void reset_player_turns(Character *protagonists, int num_protagonists);
int all_characters_have_acted(Character *protagonists, int num_protagonists);
void use_single_target_skill(Character *user, int enemy_index);
void use_aoe_skill(Character *user);
int is_enemy_alive(int enemy_index);
#endif