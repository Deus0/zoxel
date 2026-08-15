// NOTE: Toggles the meshes beased on render depth
zox_sys2(ChunkMeshToggleSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_in(ChunkLodDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(ChunkLodDirty, dirty);
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
                            zox_add(old_mesh, Disabled);
                        }
                        old_mesh = mesh;
                    }
                }
            }
        }
        // Enable to make sure it starts updating!
        if (new_mesh) {
            zox_remove(new_mesh, Disabled);
            if (dbg_log) {
                zox_log("Chunk [%s] Disabled Mesh [%s] [%i]", zox_getn(e), zox_getn(new_mesh), zox_has(new_mesh, Disabled));
            }
            byte busy =
                zox_has(new_mesh, BuildMesh) ||
                zox_has(new_mesh, MeshDirty) ||
                zox_has(new_mesh, MeshColorsGenerate) ||
                !zox_has(new_mesh, MeshBuilt);
                // !zox_getv(new_mesh, MeshRenderCount);
            if (busy) {
                if (dbg_log >= 2) {
                    zox_log("Mesh is busy [%s] BuildMesh [%i] MeshDirty [%i] MeshColorsGenerate [%i]",
                        zox_getn(new_mesh),
                        zox_has(new_mesh, BuildMesh),
                        zox_has(new_mesh, MeshDirty),
                        zox_has(new_mesh, MeshColorsGenerate)
                    );
                }
                continue;
            }
        }
        // can we just set another flag, then fade it
        if (old_mesh) {
            zox_add(old_mesh, Disabled);
            if (dbg_log) {
                zox_log("Chunk [%s] Set Mesh [%s] to Active [%i]", zox_getn(e), zox_getn(old_mesh), 0);
            }
        }
        zox_remove(e, ChunkLodDirty);
    }
} zox_sys_end(ChunkMeshToggleSystem);

