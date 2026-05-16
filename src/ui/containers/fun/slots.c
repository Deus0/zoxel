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
    }
    return e;
}
