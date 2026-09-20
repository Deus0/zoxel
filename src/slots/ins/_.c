// NOTE: Spawns slots that link to datas
// NOTE: Spawns slots that link to datas
entity spawn_slot(
    ecs* world,
    entity parent,
    uint index)
{
    entity e = zox_ins(world, prefab_slot);
    zox_set_parent(world, e, parent);
    zox_name("slot");
    zox_setv(e, ChildIndex, index);
    return e;
}

void add_more_slots(ecs* world, entity e, byte count) {
    for (byte i = 0; i < count; i++) {
        spawn_slot(world, e, i);
    }
}

entity spawn_slots(ecs* world, entity parent, byte count) {
    entity e = zox_new();
    zox_add(e, SlotManager);
    zox_set_parent(world, e, parent);
    for (byte i = 0; i < count; i++) {
        spawn_slot(world, e, i);
    }
    return e;
}
