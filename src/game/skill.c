#include "../../include/models/skill.h"
#include "../../include/models/enemy.h" // Assuming this contains Enemy enemy[MAX_ENEMIES]
#include "../../include/models/character.h"
#include "../../include/uart0.h"

extern EnemyModel enemy[MAX_ENEMIES]; // Access the enemy array

#define FIREBOLT_DAMAGE 25
#define EARTHQUAKE_DAMAGE 15
#define MAX_ENEMIES 3

void apply_skill_effect(SkillSelectionResult result) {
    if (result.action == ATTACK_SINGLE) {
        int idx = result.selected_enemy;
    if (idx >= 0 && idx < MAX_ENEMIES) {
        uart_puts("[SKILL] Firebolt hits ");
        uart_puts(enemy[idx].name);
        uart_puts(" for ");
        uart_dec(FIREBOLT_DAMAGE);
        uart_puts(" damage!\n");
        enemy[idx].current_hp -= FIREBOLT_DAMAGE;
        if (enemy[idx].current_hp < 0) enemy[idx].current_hp = 0;

        uart_puts("[ENEMY] ");
        uart_puts(enemy[idx].name);
        uart_puts(" HP is now ");
        uart_dec(enemy[idx].current_hp);
        uart_puts("\n");
    }
    } else if (result.action == ATTACK_ALL) {
        uart_puts("[SKILL] Earthquake hits all enemies for ");
        uart_dec(EARTHQUAKE_DAMAGE);
        uart_puts(" damage!\n");

        for (int i = 0; i < MAX_ENEMIES; ++i) {
            enemy[i].current_hp -= EARTHQUAKE_DAMAGE;
            if (enemy[i].current_hp < 0) enemy[i].current_hp = 0;

            uart_puts("[ENEMY] ");
            uart_puts(enemy[i].name);
            uart_puts(" HP is now ");
            uart_dec(enemy[i].current_hp);
            uart_puts("\n");
        }
    }
}

SkillSelectionResult handle_skill_selection() {
    SkillSelectionResult result = { NO_ACTION, -1 };

    uart_puts("\n[SKILL MENU]\n");
    uart_puts("1. Firebolt (Single Target)\n");
    uart_puts("2. Earthquake (AOE)\n");
    uart_puts("Select a skill: ");

    char key = uart_getc();
    // uart_puts(key);  // Echo key press

    if (key == 'o') {
        result.action = ATTACK_SINGLE;
        uart_puts("\nSelect an enemy to attack (use keys 'i' for left, 'p' for right, press Enter to confirm):\n");

        int selected = 0;
        uart_puts("Selected enemy index: 0\n");

        while (1) {
            char k = uart_getc();
            if (k == 'i' && selected > 0) {
                selected--;
            } else if (k == 'p' && selected < 2) {
                selected++;
            } else if (k == '\n' || k == '\r') {
                break;
            }
        }

        result.selected_enemy = selected;

    } else if (key == 'l') {
        result.action = ATTACK_ALL;
    }

    return result;
}