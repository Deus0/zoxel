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

// slots after those it has, numbered on from them, non fragmenting as a character's are (spawn_slots): fragmenting
//  and numbered from 0 they were missed, no items given them (tst all_items)
void add_more_slots(ecs* world, entity e, byte count) {
    const uint had = zox_get_children_count_by_id(world, e, zox_id(Slot));
    for (byte i = 0; i < count; i++) {
        spawn_slot_non_fragment(world, e, had + i);
    }
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
