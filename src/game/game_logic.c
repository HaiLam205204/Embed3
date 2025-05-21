#include "../../include/game_logic.h"
#include "../../include/models/enemy.h"
#include "../../include/bitmaps/enemy1.h"
#include "../../include/bitmaps/enemy2.h"
#include "../../include/uart0.h"
#include "../../include/utils.h"

int num_enemies = 3;  // <- THIS IS THE DEFINITION

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
    uart_dec(enemy_sprites[i].pos_x); // assuming `id` or similar exists
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
    recalculate_enemy_sprite_positions();
}

void recalculate_enemy_sprite_positions() {
    int base_x = 370;
    int spacing = 100;

    for (int i = 0; i < num_enemies; i++) {
        enemy_sprites[i].pos_x = base_x + spacing * i;
    }
}
