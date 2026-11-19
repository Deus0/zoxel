// NOTE: A block item used (Activate, its press): aimed at terrain in range its user's place swing starts, the block
//  placed at the swing's hit (item_place, eve/place.c)
void item_activate_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Activate);
    zox_sys_in(Quantity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Activate, activate);
        zox_sys_i(Quantity, quantity);
        if (activate->value != zox_dirty_active ||
            !quantity->value)
        {
            continue;
        }
        entity block = zox_get_link(world, e, BlockLink);
        if (!zox_valid(block) ||
            !zox_has(block, BlockIndex))
        {
            zox_loge("invalid block [%s]",
                zox_getn(block));
            continue;
        }
        entity user = zox_get_parent(world, e);
        const RaycastVoxelData* raycast_data = zox_get(user, RaycastVoxelData);
        const RaycastRange* range = zox_get(user, RaycastRange);
        byte hit_block = raycast_data->result == rayhit_terrain;
        byte in_range = raycast_data->distance <= range->value;
        if (!hit_block ||
            !in_range)
        {
            continue;
        }
        // the swing: the block goes in at its hit (item_place, eve/place.c), where they aim then
        skeleton_swing_begin(world, user, e, zox_current_time, zox_getv(e, SwingTime), zox_swing_place, raycast_data->hit);
    }
    zox_sys_on_end();
} zoxd_system(item_activate_system);

// NOTE: A block item's place, at its swing's hit: into the terrain where its user aims now (the cell before the hit)
void item_place(ecs* world, entity e, entity user) {
    if (!zox_valid(e) || !zox_valid(user) || !zox_getv(e, Quantity) || !zox_has(user, RaycastVoxelData)) {
        return;
    }
    const entity block = zox_get_link(world, e, BlockLink);
    if (!zox_valid(block) || !zox_has(block, BlockIndex)) {
        return;
    }
    const RaycastVoxelData* raycast_data = zox_get(user, RaycastVoxelData);
    if (raycast_data->result != rayhit_terrain || raycast_data->distance > zox_getv(user, RaycastRange)) {
        return;
    }
    byte block_index = zox_getv(block, BlockIndex);
    // Place Block
    byte3 positionl = raycast_data->positionl_last;
    entity chunk = raycast_data->chunk_last;
    if (!zox_valid(chunk) ||
        !zox_has(chunk, VoxelNodeQueue)
    ) {
        zox_loge("Invalid rayhit chunk on user [%s]",
            zox_getn(user));
        return;
    }
    // a bed or door fills its box, refused unless it's all air (multi_block_place)
    const entity terrain = zox_get_parent(world, chunk);
    // turned to the way the placer faces
    const int3 size = zox_has(block, BlockSize) ? zox_getv(block, BlockSize) : (int3) { 1, 1, 1 };
    const byte direction = block_direction_of_placer(world, user, size);
    // the cell aimed at its near end, it sticks out away
    const int3 base = block_base_from_target(chunk_block_position(world, chunk, positionl), size, direction);
    if (!multi_block_place(world, terrain, zox_get_parent(world, terrain), base, block_index, direction)) {
        spawn_sound_effect(world, sound_effect_out_of_range, user);
        return;
    }
    zox_getm(e, Quantity)->value--;
    zox_add(e, QuantityDirty);
    zox_add(e, Dirty);
    zox_add(e, DataDirty);
    // place block sound
    spawn_sound_effect(world, sound_effect_place, user);
}
