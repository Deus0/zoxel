// NOTE: This is what sets a Vox's bounds for Physics use
zox_sys2(Bounds3GrowSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNodeDirty);
    zox_sys_in(ChunkSize);
    zox_sys_in(BlockScale);
    zox_sys_out(Bounds3D);
    zox_sys_out(Bounds3Dirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNodeDirty, state);
        zox_sys_i(ChunkSize, size);
        zox_sys_i(BlockScale, scale);
        zox_sys_o(Bounds3D, bounds);
        zox_sys_o(Bounds3Dirty, dirty);
        if (state->value != zox_dirty_active) {
            continue;
        }
        float3 old_bounds = bounds->value;
        float3 new_bounds = calculate_vox_bounds(size->value, scale->value);
        if (float3_equals(old_bounds, new_bounds)) {
            continue;
        }
        bounds->value = new_bounds;
        dirty->value = zox_dirty_trigger;
        if (dbg_log) {
            zox_log("[%s] Growing Bounds [%fx%fx%f]", zox_get_name(e), new_bounds.x, new_bounds.y, new_bounds.z);
        }
        // Of entity is a moving one
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
        /*zox_sys_world();
        zox_sys_e();
        if (zox_has(e, DisableMovement)) {
            zox_geter_value(e, Acceleration3D, float3, vel);

            zox_log("Enabling Character %s", zox_get_name(e));
            zox_log("   bounds %fx%fx%f", nbounds.x, nbounds.y, nbounds.z); zox_log("   vel %fx%fx%f", vel.x, vel.y, vel.z);
        }*/
    }
} zox_sys_end(Bounds3GrowSystem);
