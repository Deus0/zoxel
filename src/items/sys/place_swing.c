// NOTE: A block item's warmup starts its user's place swing (the block lifted, then pressed out where they look), so
//  it lands as the block is placed (activate.c, the warmup's end), not after
//  - only aimed at terrain in range, as placing needs (activate.c)
void item_place_swing_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(WarmupAt);
    zox_sys_in(WarmupTime);
    zox_sys_in(CooldownTime);
    zox_sys_in(Quantity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(WarmupAt, at);
        zox_sys_i(WarmupTime, warmup);
        zox_sys_i(CooldownTime, cooldown);
        zox_sys_i(Quantity, quantity);
        if (!at->value || !quantity->value) {
            continue;
        }
        const entity user = zox_get_parent(world, e);
        if (!zox_valid(user) || !zox_has(user, RaycastVoxelData) || !zox_has(user, RaycastRange)) {
            continue;
        }
        const RaycastVoxelData* raycast = zox_get(user, RaycastVoxelData);
        if (raycast->result != rayhit_terrain || raycast->distance > zox_getv(user, RaycastRange)) {
            continue;
        }
        skeleton_swing_begin(world, user, at->value, warmup->value + cooldown->value, zox_swing_place);
    }
    zox_sys_on_end();
} zoxd_system(item_place_swing_system);
