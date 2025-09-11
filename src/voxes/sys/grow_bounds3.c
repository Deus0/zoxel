void Bounds3GrowSystem(iter *it) {
    zox_sys_begin();
    zox_sys_in(MeshDirty);
    zox_sys_in(ChunkSize);
    zox_sys_in(BlockScale);
    zox_sys_out(Bounds3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(MeshDirty, state);
        zox_sys_i(ChunkSize, size);
        zox_sys_i(BlockScale, scale);
        zox_sys_o(Bounds3D, bounds);

        if (state->value != mesh_state_end) {
            continue;
        }

        bounds->value = calculate_vox_bounds(
            size->value,
            scale->value
        );
    }
} zoxd_system2(Bounds3GrowSystem);
