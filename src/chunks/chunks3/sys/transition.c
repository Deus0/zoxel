extern byte zox_chunk_lighting_busy(ecs*, entity);
extern byte zox_chunk_mesh_lighting_busy(ecs*, entity);

// a chunk's mesh at a depth, 0 if it has none
static inline entity chunk_mesh_at_depth(ecs* world, entity chunk, byte depth) {
    iter it = zox_children(world, chunk);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity mesh = it.entities[i];
            if (zox_has(mesh, ChunkMesh) && zox_getv(mesh, RenderDepth) == depth) {
                zox_children_end(it);
                return mesh;
            }
        }
    }
    return 0;
}

// built, on the gpu, lit and its colours on the gpu too (an unlit one drew dark for a frame)
static inline byte chunk_mesh_ready(ecs* world, entity chunk, entity mesh) {
    return zox_has(mesh, MeshBuilt) &&
        !zox_has(mesh, BuildMesh) &&
        !zox_has(mesh, MeshDirty) &&
        !zox_has(mesh, MeshColorsDirty) &&
        !zox_chunk_mesh_lighting_busy(world, mesh) &&
        !zox_chunk_lighting_busy(world, chunk);
}

// shows only this mesh: enables it, disables its chunk's others, links it as the ActiveMesh
static inline void chunk_mesh_show(ecs* world, entity chunk, entity target) {
    const entity active = zox_get_link(world, chunk, ActiveMesh);
    if (active != target) {
        if (zox_valid(active)) {
            zox_unlink(world, chunk, ActiveMesh, active);
        }
        zox_link(world, chunk, ActiveMesh, target);
    }
    if (zox_has(target, Disabled)) {
        zox_remove(target, Disabled);
    }
    iter it = zox_children(world, chunk);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity mesh = it.entities[i];
            if (mesh != target && zox_has(mesh, ChunkMesh) && !zox_has(mesh, Disabled)) {
                zox_add(mesh, Disabled);
            }
        }
    }
}

// swaps a chunk to its mesh at its RenderDepth once that's ready, what it shows drawing until then (doc/done/streaming.md)
//  - what's shown is what isn't Disabled (the renderer's truth): the ActiveMesh link lagged (chunk_mesh_spawn_system
//    enables meshes it spawns), and hiding a mesh shown but not linked was a hole
//  - the target is found from the depth every frame: a depth changing back mid swap (3 to 2 to 3) swapped to a stale one
//  - shown already: it stays, the others go once it's ready; hidden: it builds hidden (BuildDisabled off), the shown ones
//    stop building (relit at their old depth they flickered dark), then it swaps in one step once chunk_mesh_ready
//  - was: a step a frame and two 0.1s timers restarting while busy (fudges over the flicker, tst lod_swap catches it)
void chunk_mesh_transition_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkLodDirty);
    zox_sys_in(RenderDepth);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChunkLodDirty, dirty);
        zox_sys_i(RenderDepth, depth);
        if (dirty->value != zox_chunk_lod_dirty_toggle) {
            continue;
        }
        const entity target = chunk_mesh_at_depth(world, e, depth->value);
        const entity preparing = zox_get_link(world, e, PreparingMesh);
        if (preparing != target && zox_valid(preparing)) {
            zox_unlink(world, e, PreparingMesh, preparing);
        }
        // air at this depth, nothing to show
        if (!target) {
            zox_remove(e, ChunkLodDirty);
            continue;
        }
        if (zox_has(target, BuildDisabled)) {
            zox_remove(target, BuildDisabled);
            // merges after this frame, its ready check next frame
            if (preparing != target) {
                zox_link(world, e, PreparingMesh, target);
            }
            continue;
        }
        const byte ready = chunk_mesh_ready(world, e, target);
        if (!zox_has(target, Disabled)) {
            // shown already: once it's ready nothing else should be
            if (ready) {
                if (zox_valid(preparing)) {
                    zox_unlink(world, e, PreparingMesh, preparing);
                }
                chunk_mesh_show(world, e, target);
                zox_remove(e, ChunkLodDirty);
            }
            continue;
        }
        if (preparing != target) {
            zox_link(world, e, PreparingMesh, target);
        }
        // what's shown stops building while the target builds
        iter mesh_it = zox_children(world, e);
        while (zox_children_next(mesh_it)) {
            for (int j = 0; j < mesh_it.count; j++) {
                const entity mesh = mesh_it.entities[j];
                if (mesh != target && zox_has(mesh, ChunkMesh) && !zox_has(mesh, Disabled) && !zox_has(mesh, BuildDisabled)) {
                    zox_add(mesh, BuildDisabled);
                }
            }
        }
        if (!ready) {
            continue;
        }
        zox_unlink(world, e, PreparingMesh, target);
        chunk_mesh_show(world, e, target);
        zox_remove(e, ChunkLodDirty);
    }
    zox_sys_on_end();
} zoxd_system(chunk_mesh_transition_system);

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
