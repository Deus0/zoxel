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
    zox_sys_in(RenderDepth);
    zox_sys_out(ChunkMeshTimer);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChunkLodDirty, dirty);
        zox_sys_i(RenderDepth, depth);
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
            // NOTE: Find target mesh and start build
            // entity lod_mesh = 0;
            iter it2 = zox_children(world, e);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count && !preparing_mesh; j++) {
                    entity mesh = it2.entities[j];
                    if (!zox_has(mesh, ChunkMesh)) {
                        continue;
                    }
                    if (zox_getv(mesh, RenderDepth) == depth->value) {
                        preparing_mesh = mesh;
                        break;
                    }
                }
                if (preparing_mesh) {
                    break;
                }
            }
            if (!zox_valid(preparing_mesh)) {
                zox_remove(e, ChunkLodDirty);
                continue;
            } else {
                zox_link(world, e, PreparingMesh, preparing_mesh);
            }
        }        entity active_mesh = zox_get_link(world, e, ActiveMesh);

        if (active_mesh == preparing_mesh) {
            // NOTE: Target mesh is already active, nothing to transition.
            if (zox_has(preparing_mesh, BuildDisabled)) {
                zox_remove(preparing_mesh, BuildDisabled);
            }
            if (zox_has(preparing_mesh, Disabled)) {
                zox_remove(preparing_mesh, Disabled);
            }
            zox_unlink(world, e, PreparingMesh, preparing_mesh);
            zox_remove(e, ChunkLodDirty);
            continue;
        }
        // Make sure new mesh is building
        if (!zox_has(preparing_mesh, Disabled)) {
            zox_add(preparing_mesh, Disabled);
            continue;
        }
        if (zox_has(preparing_mesh, BuildDisabled)) {
            zox_remove(preparing_mesh, BuildDisabled);
            timer->value = zox_current_time;
            continue;
        }

        // Make sure old mesh is not building
        // It actually tries to update lighting of it and flickers dark
        if (zox_valid(active_mesh) &&
            !zox_has(active_mesh, BuildDisabled))
        {
            timer->value = zox_current_time;
            zox_add(active_mesh, BuildDisabled);
            continue;
        }
        // Make sure old mesh is not building
        // It actually tries to update lighting of it and flickers dark
        if (zox_valid(active_mesh) &&
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
        // Reset timer when still building
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
        zox_unlink(world, e, PreparingMesh, preparing_mesh);
        if (zox_valid(active_mesh)) {
            zox_unlink(world, e, ActiveMesh, active_mesh);
        }
        active_mesh = preparing_mesh;
        zox_link(world, e, ActiveMesh, active_mesh);
        // Enable the new active mesh
        if (zox_has(active_mesh, Disabled)) {
            zox_remove(active_mesh, Disabled);
        }
        // Disable every other mesh belonging to this chunk
        iter mesh_it = zox_children(world, e);
        while (zox_children_next(mesh_it)) {
            for (int j = 0; j < mesh_it.count; j++) {
                entity mesh = mesh_it.entities[j];
                if (!zox_has(mesh, ChunkMesh) ||
                    mesh == active_mesh)
                {
                    continue;
                }
                if (!zox_has(mesh, Disabled)) {
                    zox_add(mesh, Disabled);
                }
            }
        }
        // Minimal
        zox_remove(e, ChunkLodDirty);
    }
} zox_sys_end(ChunkMeshTransitionSystem);

// can we just set another flag, then fade it
/*if (active_mesh) {
    zox_add(active_mesh, Disabled);
    if (dbg_log) {
        zox_log("Chunk [%s] Set Mesh [%s] to Active [%i]",
            zox_getn(e),
            zox_getn(active_mesh),
            0);
    }
}*/
