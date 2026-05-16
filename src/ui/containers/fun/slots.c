// NOTE: Spawns slots that link to datas
entity spawn_ui_slots(ecs* world, entity parent, byte count) {
    entity e = zox_new();
    zox_add_tag(e, SlotManager);
    zox_set_parent(world, e, parent);
    for (byte i = 0; i < count; i++) {
        entity e2 = zox_new();
        zox_set_parent(world, e2, e);
        zox_add_tag(e2, Slot);
        zox_set(e2, DataLink, { 0 });
        zox_set(e2, DataDirty, { 0 });
    }
    return e;
}

void zox_print_slots(ecs* world, entity slots_manager) {
    entity slots[layouts2_children_capacity];
    uint slots_length = zox_get_children_by_id(world, slots_manager, slots, layouts2_children_capacity, zox_id(Slot));
    for (uint k = 0; k < slots_length; k++) {
        entity slot_data = zox_gett_value(slots[k], DataLink);
        zox_log("Slot [%i] Data: %s", k, zox_get_name(slot_data));
    }
}

entity zox_get_empty_slot(ecs* world, entity slots_manager) {
    entity slots[layouts2_children_capacity];
    uint slots_length = zox_get_children_by_id(world, slots_manager, slots, layouts2_children_capacity, zox_id(Slot));
    for (uint k = 0; k < slots_length; k++) {
        entity slot = slots[k];
        entity slot_data = zox_gett_value(slot, DataLink);
        if (!slot_data) {
            return slot;
        }
    }
    return 0;
}

entity zox_get_slot_by_data(ecs* world, entity slots_manager, entity data) {
    entity slots[layouts2_children_capacity];
    uint slots_length = zox_get_children_by_id(world, slots_manager, slots, layouts2_children_capacity, zox_id(Slot));
    for (uint k = 0; k < slots_length; k++) {
        entity slot = slots[k];
        entity slot_data = zox_gett_value(slot, DataLink);
        if (slot_data == data) {
            return slot;
        }
    }
    return 0;
}
