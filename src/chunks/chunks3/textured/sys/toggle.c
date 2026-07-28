// NOTE: Toggles the meshes beased on render depth
zox_sys2(ChunkMeshToggleSystem) {
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
                    } else if (!zox_has(mesh, Disabled)) {
                        // NOTE: This happens if it switches lods too fast
                        if (old_mesh) {
                            // zox_loge("[%s] has more than one Active Meshes [%s] [%s]", zox_getn(e), zox_getn(mesh), zox_getn(old_mesh));
                            // zox_setv(old_mesh, Active, 0);
                            zox_add_tag(old_mesh, Disabled);
                        }
                        old_mesh = mesh;
                    }
                }
            }
        }
        // Enable to make sure it starts updating!
        if (new_mesh) {
            zox_remove(new_mesh, Disabled);
            // zox_setv(new_mesh, Active, 1);
            // zox_setm(new_mesh, Active, 1);
            if (dbg_log) {
                zox_log("Chunk [%s] Set Mesh [%s] to Active [%i]", zox_getn(e), zox_getn(new_mesh), 1);
            }
            // entity new_chunk = zox_get_parent(world, new_mesh);
            byte busy =
                zox_getv(new_mesh, BuildMesh) ||
                zox_getv(new_mesh, MeshDirty) ||
                zox_getv(new_mesh, MeshColorsGenerate);
            if (busy) {
                continue;
            }
        }
        // can we just set another flag, then fade it
        if (old_mesh) {
            zox_add_tag(old_mesh, Disabled);
            // zox_setv(old_mesh, Active, 0);
            // zox_setm(old_mesh, Active, 0);
            if (dbg_log) {
                zox_log("Chunk [%s] Set Mesh [%s] to Active [%i]", zox_getn(e), zox_getn(old_mesh), 0);
            }
        }
        dirty->value = zox_chunk_lod_dirty_end;
    }
} zox_sys_end(ChunkMeshToggleSystem);

