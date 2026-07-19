// When Mesh Rebuilds, rebuild our colors too based on lights
// If queue empty + lightnode dirty, activate!
zox_sys2(ChunkColorsTriggerSystem) {
    if (disable_lights) {
        return;
    }
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LightNodeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LightNodeDirty, lights_dirty);
        if (lights_dirty->value != zox_dirty_active) {
            continue;
        }
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                if (zox_has(e2, ChunkMesh)) {
                    zox_set(e2, MeshColorsGenerate, { 1 });
                    if (dbg_log) {
                        zox_log("Chunk Triggered Build [%s] > [%s]:[%s]", zox_getn(e), zox_getn(e2));
                    }
                }
            }
        }
    }
} zox_sys_end(ChunkColorsTriggerSystem);

// TODO: There is no delay so it might get skipped
zox_sys2(ChunkMeshColorsTriggerSystem) {
    if (disable_lights) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(BuildChunkMesh);
    zox_sys_out(MeshColorsGenerate);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(BuildChunkMesh, build);
        zox_sys_o(MeshColorsGenerate, generate);
        if (build->value) {
            generate->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(ChunkMeshColorsTriggerSystem);
