#include "../../include/models/skill.h"
#include "../../include/models/enemy.h" // Assuming this contains Enemy enemy[MAX_ENEMIES]
#include "../../include/models/character.h"
#include "../../include/uart0.h"

extern Enemy enemy[MAX_ENEMIES]; // Access the enemy array

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
        uart_putint(FIREBOLT_DAMAGE);
        uart_puts(" damage!\n");
        enemy[idx].current_hp -= FIREBOLT_DAMAGE;
        if (enemy[idx].current_hp < 0) enemy[idx].current_hp = 0;

        uart_puts("[ENEMY] ");
        uart_puts(enemy[idx].name);
        uart_puts(" HP is now ");
        uart_putint(enemy[idx].current_hp);
        uart_puts("\n");
    }
    } else if (result.action == ATTACK_ALL) {
        uart_puts("[SKILL] Earthquake hits all enemies for ");
        uart_putint(EARTHQUAKE_DAMAGE);
        uart_puts(" damage!\n");

        for (int i = 0; i < MAX_ENEMIES; ++i) {
            enemy[i].current_hp -= EARTHQUAKE_DAMAGE;
            if (enemy[i].current_hp < 0) enemy[i].current_hp = 0;

            uart_puts("[ENEMY] ");
            uart_puts(enemy[i].name);
            uart_puts(" HP is now ");
            uart_putint(enemy[i].current_hp);
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

            uart_puts("Selected enemy index: ");
            uart_putint(selected);
            uart_puts("\n");
        }

        result.selected_enemy = selected;

    } else if (key == 'l') {
        result.action = ATTACK_ALL;
    }

    return result;
}

// void draw_skill_option_terminal(int skill_index){
//     uart_puts("\n=== Skill Menu ===\n");
//     for (int i = 0; i < MAX_SKILLS; ++i) {
//         if (i == skill_index) uart_puts("-> ");
//         else uart_puts(" ");
//         uart_puts(skills[i].name);
//         uart_puts("\n");
//     }
//     uart_puts("==================\n");
// }

// void use_skill(Character* user, Skill skill) {
//     if (skill.type == SKILL_ALL_ENEMIES) {
//     for (int i = 0; i < 3; ++i) {
//     enemy[i].current_hp -= skill.damage;
//     if (enemy[i].current_hp < 0) enemy[i].current_hp = 0;

//             uart_puts("[SKILL] ");
//             uart_puts(user->name);
//             uart_puts(" hit ");
//             uart_puts(enemy[i].name);
//             uart_puts(" for ");
//             uart_putint(skill.damage);
//             uart_puts(" damage! New HP: ");
//             uart_putint(enemy[i].current_hp);
//             uart_puts("\n");
//         }
//     } else {
//         // For now, we will just hit enemy[0]
//         enemy[0].current_hp -= skill.damage;
//         if (enemy[0].current_hp < 0) enemy[0].current_hp = 0;

//         uart_puts("[SKILL] ");
//         uart_puts(user->name);
//         uart_puts(" used ");
//         uart_puts(skill.name);
//         uart_puts(" on ");
//         uart_puts(enemy[0].name);
//         uart_puts(" for ");
//         uart_putint(skill.damage);
//         uart_puts(" damage! New HP: ");
//         uart_putint(enemy[0].current_hp);
//         uart_puts("\n");
//     }
// }