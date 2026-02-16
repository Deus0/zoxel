zox_sys2(Bounds3GrowSystem) {
    zox_sys_begin();
    zox_sys_in(MeshDirty);
    zox_sys_in(ChunkSize);
    zox_sys_in(BlockScale);
    zox_sys_out(Bounds3D);
    zox_sys_out(Bounds3Dirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(MeshDirty, state);
        zox_sys_i(ChunkSize, size);
        zox_sys_i(BlockScale, scale);
        zox_sys_o(Bounds3D, bounds);
        zox_sys_o(Bounds3Dirty, dirty);

        if (state->value != mesh_state_end) {
            continue;
        }

        float3 nbounds = calculate_vox_bounds(
            size->value,
            scale->value
        );

        if (float3_equals(bounds->value, nbounds)) {
            continue;
        }

        bounds->value = nbounds;
        dirty->value = zox_dirty_trigger;

        /*zox_sys_world();
        zox_sys_e();
        if (zox_has(e, DisableMovement)) {
            zox_geter_value(e, Acceleration3D, float3, vel);

            zox_log("Enabling Character %s", zox_get_name(e));
            zox_log("   bounds %fx%fx%f", nbounds.x, nbounds.y, nbounds.z); zox_log("   vel %fx%fx%f", vel.x, vel.y, vel.z);
        }*/

    }
} zox_sys_end(Bounds3GrowSystem);
