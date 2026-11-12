// Sets new bounds, moving physics objects up by the difference so they stay grounded
//  - returns 1 if bounds changed
byte grow_bounds3(
    ecs* world,
    entity e,
    Bounds3D* bounds,
    Bounds3Dirty* dirty,
    float3 new_bounds)
{
    float3 old_bounds = bounds->value;
    if (float3_equals(old_bounds, new_bounds)) {
        return 0;
    }
    bounds->value = new_bounds;
    dirty->value = zox_dirty_trigger;
    // NOTE: If Physics Object, we reposition based on new offset upwards!
    if (zox_has(e, Position3D) && zox_has(e, Moveable)) {
        // Assume that gravity is downwards for now
        // If new bounds is greater, we move the entity up!
        float difference_y = new_bounds.y - old_bounds.y;
        zox_muter(e, Position3D, position);
        position->value.y += difference_y;
        zox_muter(e, LastPosition3D, last_position);
        last_position->value.y += difference_y;
    }
    return 1;
}

// NOTE: This is what sets a Vox's bounds for Physics use
void bounds3_grow_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkSize);
    zox_sys_in(BlockScale);
    zox_sys_out(Bounds3D);
    zox_sys_out(Bounds3Dirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChunkSize, size);
        zox_sys_i(BlockScale, scale);
        zox_sys_o(Bounds3D, bounds);
        zox_sys_o(Bounds3Dirty, dirty);
        float3 new_bounds = calculate_vox_bounds(size->value, scale->value);
        if (grow_bounds3(world, e, bounds, dirty, new_bounds) && dbg_log) {
            zox_log("[%s] Growing Bounds [%fx%fx%f]", zox_get_name(e), new_bounds.x, new_bounds.y, new_bounds.z);
        }
    }
    zox_sys_on_end();
} zoxd_system(bounds3_grow_system);

void bounds3_enable_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Bounds3Dirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Bounds3Dirty, updated);
        if (updated->value == zox_dirty_active) {
            zox_remove(e, DisableMovement);
        }
    }
    zox_sys_on_end();
} zoxd_system(bounds3_enable_system);


// Grows bounds to fit a child vox (VoxLink), when its voxels change, i.e. for characters
void vox_link_bounds3_grow_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(Bounds3D);
    zox_sys_out(Bounds3Dirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(Bounds3D, bounds);
        zox_sys_o(Bounds3Dirty, dirty);
        const entity vox = zox_get_link(world, e, VoxLink);
        if (!zox_valid(vox) || !zox_has(vox, VoxelNodePostDirty)) {
            continue;
        }
        float3 new_bounds = calculate_vox_bounds(
            zox_getv(vox, ChunkSize),
            zox_getv(vox, BlockScale));
        if (grow_bounds3(world, e, bounds, dirty, new_bounds) && dbg_log) {
            zox_log("[%s] Bounds from Vox [%fx%fx%f]", zox_sys_e_name, new_bounds.x, new_bounds.y, new_bounds.z);
        }
    }
    zox_sys_on_end();
} zoxd_system(vox_link_bounds3_grow_system);
