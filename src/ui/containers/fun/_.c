void zox_print_slots(ecs* world, entity slots_manager) {
    entity slots[layouts2_children_capacity];
    uint slots_length = zox_get_children_by_id(world, slots_manager, slots, layouts2_children_capacity, zox_id(Slot));
    for (uint k = 0; k < slots_length; k++) {
        entity slot_data = zox_getv(slots[k], DataLink);
        zox_log("Slot [%i] Data: %s", k, zox_get_name(slot_data));
    }
}

entity zox_get_empty_slot(ecs* world, entity e) {
    /*entity slots[layouts2_children_capacity];
    uint slots_length = zox_get_children_by_id(world, e, slots, layouts2_children_capacity, zox_id(Slot));
    for (uint k = 0; k < slots_length; k++) {
        entity slot = slots[k];*/
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (!zox_valid(e2) || !zox_has(e2, Slot)) {
                continue;
            }
            entity slot_data = zox_getv(e2, DataLink);
            if (!slot_data) {
                return e2;
            }
        }
    }
    return 0;
}

entity zox_get_slot_by_data(ecs* world, entity slots_manager, entity data) {
    entity slots[layouts2_children_capacity];
    uint slots_length = zox_get_children_by_id(world, slots_manager, slots, layouts2_children_capacity, zox_id(Slot));
    for (uint k = 0; k < slots_length; k++) {
        entity slot = slots[k];
        entity slot_data = zox_getv(slot, DataLink);
        if (slot_data == data) {
            return slot;
        }
    }
    return 0;
}

uint zox_get_slots_by_data(ecs* world, entity smanager, entity base_data, entity* result, uint capacity) {
    if (capacity == 0) {
        return 0;
    }
    uint count = 0;
    entity slots[layouts2_children_capacity];
    uint slots_length = zox_get_children_by_id(world, smanager, slots, layouts2_children_capacity, zox_id(Slot));
    for (uint k = 0; k < slots_length; k++) {
        entity slot = slots[k];
        if (!zox_valid(slot) || !zox_has(slot, DataLink)) {
            continue;
        }
        entity data = zox_getv(slot, DataLink);
        if (!zox_valid(data)) {
            continue;
        }
        entity prefab = zox_get_prefab(world, data);
        if (prefab == base_data) {
            result[count++] = slot;
            if (count >= capacity) {
                zox_logw("[zox_get_stacking_slots] exceeded capacity [%i]", capacity);
                return count;
            }
        }
    }
    return count;
}
