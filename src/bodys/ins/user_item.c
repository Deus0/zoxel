entity spawn_user_item_body(ecs *world, entity user, entity base_item, entity parent, byte slot, byte3 position, byte3 center_position) {

    entity e = spawn_user_item(world, base_item, user);

    zox_set(e, SlotType, { slot });
    zox_set(e, AttachLink, { parent });
    zox_set(e, PartLinks, { 0 });
    zox_set(e, VoxelPosition, { byte3_to_int3(position) });
    zox_set(e, PartPosition, { center_position });

    return e;
}
