entity spawn_user_item_body(ecs *world, entity parent, entity base_item, byte slot) {
    entity e = spawn_user_item(world, parent, base_item);
    zox_set(e, SlotType, { slot });
    // zox_set(e, AttachLink, { parent });
    // zox_set(e, BlockPosition, { byte3_to_int3(position) });
    // zox_set(e, PartPosition, { center_position });
    // zox_set_parent(world, chest, e);
    zox_set(e, BlockPosition, { int3_zero });
    zox_set(e, PartPosition, { byte3_zero });
    return e;
}
