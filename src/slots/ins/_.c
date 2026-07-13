// NOTE: Spawns slots that link to datas
// NOTE: Spawns slots that link to datas
entity spawn_slot(ecs* world, entity parent) {
    entity e = zox_ins(world, prefab_slot);
    zox_set_parent(world, e, parent);
    zox_name("slot");
    return e;
}

void add_more_slots(ecs* world, entity e, byte count) {
    for (byte i = 0; i < count; i++) {
        spawn_slot(world, e);
    }
}

entity spawn_slots(ecs* world, entity parent, byte count) {
    entity e = zox_new();
    zox_add_tag(e, SlotManager);
    zox_set_parent(world, e, parent);
    for (byte i = 0; i < count; i++) {
        spawn_slot(world, e);
    }
    return e;
}
