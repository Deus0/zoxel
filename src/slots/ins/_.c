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

// a slot as a non fragmenting child: a character's body and equipment slots, beside its non fragmenting
//  stats (a prefab's children must be one kind, a skeleton prefab's ChildOf body slot was lost on
//  instances of humanoid prefabs with stats, invisible humanoids, tst prefab_variants, npc_skeletons)
entity spawn_slot_non_fragment(
    ecs* world,
    entity parent,
    uint index)
{
    entity e = zox_ins(world, prefab_slot);
    zox_set_parent_non_fragment(world, e, parent);
    zox_name("slot");
    zox_setv(e, ChildIndex, index);
    return e;
}

// slots for a character (actionbar, inventory, skillbook), non fragmenting like its other children (stats,
//  body slot): a character prefab's tree must be one kind of children, mixed its instances lost some
entity spawn_slots(ecs* world, entity parent, byte count) {
    entity e = zox_new();
    zox_add(e, SlotManager);
    zox_set_parent_non_fragment(world, e, parent);
    for (byte i = 0; i < count; i++) {
        spawn_slot_non_fragment(world, e, i);
    }
    return e;
}
