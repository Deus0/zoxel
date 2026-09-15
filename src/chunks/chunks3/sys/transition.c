extern byte zox_chunk_lighting_busy(ecs*, entity);
extern byte zox_chunk_mesh_lighting_busy(ecs*, entity);

// NOTE: Toggles the meshes beased on render depth
zox_sys2(ChunkMeshTransitionSystem) {
    byte dbg_log = 0;
    byte disable_busy = 0;
    double transition_speed = 1;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkLodDirty);
    zox_sys_out(ChunkMeshTimer);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChunkLodDirty, dirty);
        zox_sys_o(ChunkMeshTimer, timer);
        if (dirty->value != zox_chunk_lod_dirty_toggle) {
            continue;
        }
        if (transition_speed &&
            zox_current_time - timer->value < transition_speed) {
            continue;
        }
        entity preparing_mesh = zox_get_link(world, e, PreparingMesh);
        // Enable to make sure it starts updating!
        if (!zox_valid(preparing_mesh)) {
            // zox_loge("No Preparing Mesh %s", zox_sys_e_name);
            zox_remove(e, ChunkLodDirty);
            continue;
        }
        // Make sure new mesh is building
        if (zox_has(preparing_mesh, BuildDisabled)) {
            zox_remove(preparing_mesh, BuildDisabled);
            // timer->value = zox_current_time;
            continue;
        }
        entity active_mesh = zox_get_link(world, e, ActiveMesh);
        // Make sure old mesh is not building
        // It actually tries to update lighting of it and flickers dark
        if (active_mesh &&
            !zox_has(active_mesh, BuildDisabled))
        {
            timer->value = zox_current_time;
            zox_add(active_mesh, BuildDisabled);
            continue;
        }
        // We can instead just let them build by having a seperate tag here?
        if (dbg_log) {
            zox_log("Chunk [%s] Disabled Mesh [%s] [%i]",
                zox_getn(e),
                zox_getn(preparing_mesh),
                zox_has(preparing_mesh, Disabled));
        }
        byte busy = disable_busy ? 0 : (
            // NOTE: Checks queues
            zox_has(preparing_mesh, BuildMesh) ||
            zox_has(preparing_mesh, MeshDirty) ||
            !zox_has(preparing_mesh, MeshBuilt) ||
            zox_chunk_lighting_busy(world, e) ||
            zox_chunk_mesh_lighting_busy(world, preparing_mesh)
        );
        if (busy) {
            timer->value = zox_current_time;
            if (dbg_log >= 2) {
                zox_log("Mesh is busy [%s] BuildMesh [%i] MeshDirty [%i] BuildMeshColors [%i]",
                    zox_getn(preparing_mesh),
                    zox_has(preparing_mesh, BuildMesh),
                    zox_has(preparing_mesh, MeshDirty),
                    zox_has(preparing_mesh, BuildMeshColors)
                );
            }
            continue;
        }
        if (active_mesh) {
            zox_unlink(world, e, ActiveMesh, active_mesh);
        }
        zox_unlink(world, e, PreparingMesh, preparing_mesh);
        zox_link(world, e, ActiveMesh, preparing_mesh);
        if (zox_has(preparing_mesh, Disabled)) {
            zox_remove(preparing_mesh, Disabled);
        }
        // can we just set another flag, then fade it
        if (active_mesh) {
            zox_add(active_mesh, Disabled);
            if (dbg_log) {
                zox_log("Chunk [%s] Set Mesh [%s] to Active [%i]",
                    zox_getn(e),
                    zox_getn(active_mesh),
                    0);
            }
        }
        // Minimal
        zox_remove(e, ChunkLodDirty);
    }
} zox_sys_end(ChunkMeshTransitionSystem);