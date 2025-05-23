#include "../../include/game_logic.h"
#include "../../include/models/enemy.h"
#include "../../include/models/character.h"
#include "../../include/models/character_sprite.h"
#include "../../include/bitmaps/enemy1.h"
#include "../../include/bitmaps/enemy2.h"
#include "../../include/uart0.h"
#include "../../include/utils.h"
#include "../../include/game_design.h"
#include "../../include/game.h"
#include "../../include/game_combat.h"

int num_enemies = 3;  // <- THIS IS THE DEFINITION
int num_protagonists = 4;
void deal_damage(int index, int amount) {
    enemy[index].current_hp -= amount;
    if (enemy[index].current_hp < 0) {
        enemy[index].current_hp = 0;
    }

    uart_puts("[DEBUG] Enemy HP after attack: ");
    uart_dec(enemy[index].current_hp);
    uart_puts("\n");

    // If the enemy is dead, remove them
    if (enemy[index].current_hp == 0) {
        remove_enemy(index);
    }

    for (int i = 0; i < num_enemies; ++i) {
        draw_enemy_sprite(&enemy_sprites[i]);
        draw_enemy_sprite(&enemy_sprites[i]);
    }

    for (int i = 0; i < num_enemies; i++) {
    uart_puts("[DEBUG] Enemy: ");
    uart_puts(enemy[i].name);
    uart_puts(" Sprite pos_x: ");
    uart_dec(enemy_sprites[i].pos_x); 
    uart_puts("\n");
    }
}

void remove_enemy(int index) {
    if (index < 0 || index >= num_enemies) return;

    uart_puts("[DEBUG] Removing enemy: ");
    uart_puts(enemy[index].name);
    uart_puts("\n");

    // Shift enemy data
    for (int i = index; i < num_enemies - 1; i++) {
        enemy[i] = enemy[i + 1];
        enemy_sprites[i] = enemy_sprites[i + 1];
        enemy_sprites[i].enemy = &enemy[i];  // FIX: Update pointer to correct enemy
    }

    num_enemies--;
    // recalculate_enemy_sprite_positions();

    uart_puts("Number of enemy: ");
    uart_putint(num_enemies);
    uart_puts("\n");

    if (num_enemies == 0) {
        uart_puts("[COMBAT] All enemies defeated. Exiting to exploration...\n");
        protag_world_x -= 50; // or any direction away from the enemy
        protag_world_y -= 50;
        exit_ui = 1;
    }
}

void recalculate_enemy_sprite_positions() {
    int base_x = 370;
    int spacing = 100;

    for (int i = 0; i < num_enemies; i++) {
        enemy_sprites[i].pos_x = base_x + spacing * i;
    }
}

void enemy_turn(Character *protagonists, int num_protagonists) {
    uart_puts("[ENEMY TURN] Enemies are attacking...\n");

    for (int i = 0; i < num_enemies; ++i) {
        EnemyModel *e = &enemy[i];

        // Choose an attack type: 0 = single-target, 1 = AoE
        int attack_type = rand_0_or_1();

        if (attack_type == 0) {
            // Single-target attack
            int target_index = rand_0_to_3();
            int damage = 30;  // Example damage

            protagonists[target_index].current_hp -= damage;
                if (protagonists[target_index].current_hp <= 0) {
                    protagonists[target_index].current_hp = 0;
                    remove_protagonist(target_index);
            }
            uart_puts("[ENEMY TURN] ");
            uart_puts(e->name);
            uart_puts(" attacks ");
            uart_puts(protagonists[target_index].name);
            uart_puts(" for ");
            uart_dec(damage);
            uart_puts(" damage!\n");

        } else {
            // AoE attack
            int damage = 20;

            uart_puts("[ENEMY TURN] ");
            uart_puts(e->name);
            uart_puts(" uses AoE attack!\n");

            for (int j = num_protagonists - 1; j >= 0; --j) {
                protagonists[j].current_hp -= damage;
                if (protagonists[j].current_hp <= 0) {
                    protagonists[j].current_hp = 0;
                    remove_protagonist(j);
                }
            }
        }

        redraw_combat_screen(0, 0);
        redraw_combat_screen(0, 0);

        // print_number(attack_type);
        wait_us(2000000ULL);  // wait 2,000,000 microseconds = 2 seconds
    }
}

void reset_player_turns(Character *protagonists, int num_protagonists) {
    for (int i = 0; i < num_protagonists; ++i) {
        protagonists[i].has_acted = 0;
    }
}

int all_characters_have_acted(Character *protagonists, int num_protagonists) {
    for (int i = 0; i < num_protagonists; ++i) {
        if (protagonists[i].current_hp > 0 && protagonists[i].has_acted == 0) {
            return 0; // Someone hasn't acted yet
        }
    }
    return 1; // All have acted
}

void remove_protagonist(int index) {
    if (index < 0 || index >= num_protagonists) return;

    uart_puts("[DEBUG] Removing protagonist: ");
    uart_puts(protagonists[index].name);
    uart_puts("\n");

    // Shift the array left
    for (int i = index; i < num_protagonists - 1; i++) {
        protagonists[i] = protagonists[i + 1];
        sprites[i] = sprites[i + 1];
        sprites[i].character = &protagonists[i];  // FIX: Update pointer to correct enemy
    }

    num_protagonists--;  // reduce the count
}

void use_single_target_skill(Character *user, int enemy_index) {
    int skill_cost = 12;
    int skill_damage = 30;  // You can tweak this

    if (user->current_hp < skill_cost) {
        uart_puts("[SKILL] Not enough HP to use single-target skill.\n");
        return;
    }

    user->current_hp -= skill_cost;
    uart_puts("[SKILL] Using single-target skill. Remaining HP: ");
    uart_dec(user->current_hp);
    uart_puts("\n");

    deal_damage(enemy_index, skill_damage);
}

void use_aoe_skill(Character *user) {
    int skill_cost = 20;
    int aoe_damage = 15;  // You can tweak this

    if (user->current_hp < skill_cost) {
        uart_puts("[SKILL] Not enough HP to use AoE skill.\n");
        return;
    }

    user->current_hp -= skill_cost;
    uart_puts("[SKILL] Using AoE skill. Remaining HP: ");
    uart_dec(user->current_hp);
    uart_puts("\n");

    for (int i = num_enemies - 1; i >= 0; i--) {
        deal_damage(i, aoe_damage);
    }
}

int is_enemy_alive(int enemy_index) {
    return enemy[enemy_index].current_hp > 0;
}

int all_enemies_defeated(EnemyModel enemies[], int num_enemies) {
    for (int i = 0; i < num_enemies; i++) {
        uart_puts("[DEBUG] Enemy ");
        uart_putint(i);
        uart_puts(" HP: ");
        uart_putint(enemies[i].current_hp);
        uart_puts("\n");
        if (enemies[i].current_hp > 0) return 0;
    }
    uart_puts("[DEBUG] All enemies are defeated.\n");
    return 1;
}

int all_allies_defeated(Character allies[], int num_allies) {
    for (int i = 0; i < num_allies; i++) {
        uart_puts("[DEBUG] Ally ");
        uart_putint(i);
        uart_puts(" HP: ");
        uart_putint(allies[i].current_hp);
        uart_puts("\n");
        if (allies[i].current_hp > 0) return 0;
    }
    uart_puts("[DEBUG] All allies are defeated.\n");
    return 1;
}