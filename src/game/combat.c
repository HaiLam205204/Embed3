#include "../../include/models/character.h"
#include "../../include/models/enemy.h"
#include "../../include/combat.h"
#include "../../include/uart0.h"

void take_turn(int character_index)
{
    Character *character = &protagonists[character_index];
    uart_puts("[ACTION] ");
    uart_puts(character->name);
    uart_puts(" takes a simple action.\n");
}