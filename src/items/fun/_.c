#include "label.c"
#include "linked.c"

byte can_stack_items(ecs* world, entity a, entity b) {
    if (!zox_valid(a) || !zox_valid(b)) {
        return 0;
    }
    // Cannot stack if not stackable! aya!
    if (!zox_has(a, Quantity) || !zox_has(b, Quantity)) {
        return 0;
    }
    if (zox_getv(a, Quantity) + zox_getv(b, Quantity) > max_stack_quantity) {
        return 0;
    } else {
        return 1;
    }
}

byte stack_items(ecs* world, entity a, entity b) {
    if (!zox_valid(a) || !zox_valid(b)) {
        return 0;
    }
    // Cannot stack if not stackable! aya!
    if (!zox_has(a, Quantity) || !zox_has(b, Quantity)) {
        return 0;
    }
    byte quantity = zox_getv(a, Quantity) + zox_getv(b, Quantity);
    zox_muter(a, Quantity, a_quantity);
    a_quantity->value = quantity;
    zox_muter(b, Quantity, b_quantity);
    b_quantity->value = 0;
    zox_add(a, QuantityDirty);
    zox_add(b, QuantityDirty);
    return 1;
}

// an item's stack size, 1 if it doesn't stack, 0 if invalid
byte get_item_quantity(ecs* world, entity item) {
    if (!zox_valid(item)) {
        return 0;
    }
    return zox_has(item, Quantity) ? zox_getv(item, Quantity) : 1;
}

// a new item like item (same prefab and user), holding quantity of its own
static inline entity spawn_item_like(ecs* world, entity item, byte quantity) {
    const entity prefab = zox_get_prefab(world, item);
    if (!zox_valid(prefab)) {
        return 0;
    }
    const entity e = spawn_user_item(world, zox_get_parent(world, item), prefab);
    zox_setv(e, Quantity, quantity);
    return e;
}

// splits half of a stack (rounded down) off into a new item, returns it, 0 if it can't split
entity split_item_stack(ecs* world, entity item) {
    if (!zox_valid(item) || !zox_has(item, Quantity)) {
        return 0;
    }
    zox_muter(item, Quantity, quantity);
    if (quantity->value < 2) {
        return 0;
    }
    const byte half = quantity->value / 2;
    const entity e = spawn_item_like(world, item, half);
    if (!e) {
        return 0;
    }
    quantity->value -= half;
    zox_add(item, QuantityDirty);
    return e;
}

// takes one off a stack into a new item, returns it (the stack deletes itself at 0)
entity take_one_item(ecs* world, entity item) {
    if (!zox_valid(item) || !zox_has(item, Quantity)) {
        return 0;
    }
    zox_muter(item, Quantity, quantity);
    if (!quantity->value) {
        return 0;
    }
    const entity e = spawn_item_like(world, item, 1);
    if (!e) {
        return 0;
    }
    quantity->value--;
    zox_add(item, QuantityDirty);
    return e;
}

// moves one item from a stack onto another stack of the same item, returns 0 if it can't
byte add_one_item(ecs* world, entity to, entity from) {
    if (!zox_valid(to) || !zox_valid(from) ||
        !zox_has(to, Quantity) || !zox_has(from, Quantity) ||
        zox_get_prefab(world, to) != zox_get_prefab(world, from))
    {
        return 0;
    }
    zox_muter(to, Quantity, to_quantity);
    zox_muter(from, Quantity, from_quantity);
    if (!from_quantity->value || to_quantity->value >= max_stack_quantity) {
        return 0;
    }
    to_quantity->value++;
    from_quantity->value--;
    zox_add(to, QuantityDirty);
    zox_add(from, QuantityDirty);
    return 1;
}
