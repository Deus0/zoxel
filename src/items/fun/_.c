#include "label.c"
#include "linked.c"

byte can_stack_items(ecs* world, entity a, entity b) {
    if (!zox_valid(a) || !zox_valid(b)) {
        return 0;
    }
    if (zox_gett_value(a, Quantity) + zox_gett_value(b, Quantity) > max_stack_quantity) {
        return 0;
    } else {
        return 1;
    }
}

byte stack_items(ecs* world, entity a, entity b) {
    if (!zox_valid(a) || !zox_valid(b)) {
        return 0;
    }
    byte quantity = zox_gett_value(a, Quantity) + zox_gett_value(b, Quantity);
    zox_muter(a, Quantity, a_quantity);
    a_quantity->value = quantity;
    zox_muter(a, QuantityDirty, adirty);
    adirty->value = zox_dirty_trigger;
    zox_muter(b, Quantity, b_quantity);
    b_quantity->value = 0;
    zox_muter(b, QuantityDirty, bdirty);
    bdirty->value = zox_dirty_trigger;
    return 1;
}
