// NOTE: Toggles the meshes beased on render depth
zox_sys2(ChunkMeshToggleSystem) {
    // TODO: Only spawn mesh if sides exist!
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_out(ChunkLodDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, depth);
        zox_sys_o(ChunkLodDirty, dirty);
        if (dirty->value != zox_chunk_lod_dirty_toggle) {
            continue;
        }
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity mesh = it2.entities[j];
                if (!zox_has(mesh, ChunkMesh)) {
                    continue;
                }
                zox_set_enabled(mesh, zox_getv(mesh, RenderDepth) == depth->value);
            }
        }
        dirty->value = zox_chunk_lod_dirty_spawn;
    }
} zox_sys_end(ChunkMeshToggleSystem);
