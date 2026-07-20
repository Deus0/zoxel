// NOTE: Toggles the meshes beased on render depth
zox_sys2(ChunkMeshToggleSystem) {
    byte dbg_log = 0;
    double deactivate_delay = 2.5;
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
        entity old_mesh = 0;
        entity new_mesh = 0;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity mesh = it2.entities[j];
                if (zox_has(mesh, ChunkMesh)) {
                    byte mesh_depth = zox_getv(mesh, RenderDepth);
                    if (mesh_depth == depth->value) {
                        new_mesh = mesh;
                    } else if (zox_getv(mesh, Active)) {
                        old_mesh = mesh;
                    }
                }
            }
        }
        // Enable to make sure it starts updating!
        if (new_mesh) {
            zox_setm(new_mesh, Active, 1);
            // NOTE: Incase its still Deactivating
            zox_setm(new_mesh, DeactivateDelay, 0);
            if (dbg_log) {
                zox_log("Chunk [%s] Set Mesh [%s] to Active [%i]", zox_getn(e), zox_getn(new_mesh), 1);
            }
        }
        byte busy = new_mesh && (zox_getv(new_mesh, BuildChunkMesh) || zox_getv(new_mesh, TexturedMeshDirty));
        if (busy) {
            continue;
        }
        // can we just set another flag, then fade it
        if (old_mesh) {
            // zox_setm(old_mesh, Active, 0);
            ushort delay_one_second = deactivate_delay / zox_delta_time;
            zox_setm(old_mesh, DeactivateDelay, delay_one_second);
            if (dbg_log) {
                zox_log("Chunk [%s] Set Mesh [%s] to Active [%i] [%i]", zox_getn(e), zox_getn(old_mesh), 0, delay_one_second);
            }
        }
        dirty->value = zox_chunk_lod_dirty_end;
    }
} zox_sys_end(ChunkMeshToggleSystem);

/*byte meshes_busy = 0;
meshes_busy |= zox_getv(e, BuildChunkSides);
iter it2 = zox_children(world, e);
while (zox_children_next(it2)) {
    for (int j = 0; j < it2.count && !meshes_busy; j++) {
        entity mesh = it2.entities[j];
        if (!zox_has(mesh, ChunkMesh)) {
            continue;
        }
        if (!zox_getv(mesh, Active)) {
            // no need to wait though if inactive
            continue;
        }
        if (zox_getv(mesh, BuildChunkMesh) || zox_getv(mesh, TexturedMeshDirty)) {
            meshes_busy = 1;
            break;
        }
    }
}
for (int n = 0; n < 6 && !meshes_busy; n++) {
    entity neighbor = neighbors->value[n];
    if (!zox_valid(neighbor)) {
        continue;
    }
    meshes_busy |= zox_getv(neighbor, BuildChunkSides);
    iter it2 = zox_children(world, neighbor);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count && !meshes_busy; j++) {
            entity mesh = it2.entities[j];
            if (!zox_has(mesh, ChunkMesh)) {
                continue;
            }
            if (!zox_getv(mesh, Active)) {
                continue;
            }
            if (zox_getv(mesh, BuildChunkMesh) || zox_getv(mesh, TexturedMeshDirty)) {
                meshes_busy = 1;
                break;
            }
        }
    }
}
// Wait until they generated before swapping!
if (meshes_busy) {
    continue;
}*/
// Keep high on for now to test
/*if (dirty->value == zox_chunk_lod_dirty_enable && active) { zox_setm(mesh, Active, active);
} else if (dirty->value == zox_chunk_lod_dirty_disable && !active) {
    zox_setm(mesh, Active, active);
}*/
// zox_set_enabled(mesh, active);
