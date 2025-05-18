#ifndef ITEM_H
#define ITEM_H

#include "constants.h"

typedef struct {
    int item_quantity;
    char name[MAX_ITEM_NAME_LENGTH];
} Item;

#endif
