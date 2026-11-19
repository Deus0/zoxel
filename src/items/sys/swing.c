// NOTE: An item's warmup starts its user's swing, so it lands as the item is used (the warmup's end), not after
//  - a block: a place (the block lifted, then pressed out where they look), only aimed at terrain in range as placing
//    needs (activate.c); its point is the ray's hit, the head turns to it
//  - food: eating, the hand to the mouth (consume.c)
void item_swing_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(WarmupAt);
    zox_sys_in(Quantity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(WarmupAt, at);
        zox_sys_i(Quantity, quantity);
        if (!at->value || !quantity->value) {
            continue;
        }
        const entity user = zox_get_parent(world, e);
        if (!zox_valid(user)) {
            continue;
        }
        const float time = zox_getv(e, SwingTime);
        if (zox_has(e, ItemConsumable)) {
            skeleton_swing_begin(world, user, e, at->value, time, zox_swing_eat, float3_zero);
            continue;
        }
        if (!zox_has(e, ItemBlock) || !zox_has(user, RaycastVoxelData) || !zox_has(user, RaycastRange)) {
            continue;
        }
        const RaycastVoxelData* raycast = zox_get(user, RaycastVoxelData);
        if (raycast->result != rayhit_terrain || raycast->distance > zox_getv(user, RaycastRange)) {
            continue;
        }
        skeleton_swing_begin(world, user, e, at->value, time, zox_swing_place, raycast->hit);
    }
    zox_sys_on_end();
} zoxd_system(item_swing_system);
