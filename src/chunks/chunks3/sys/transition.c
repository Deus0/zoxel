extern byte zox_chunk_lighting_busy(ecs*, entity);
extern byte zox_chunk_mesh_lighting_busy(ecs*, entity);

// built, on the gpu, lit and its colours on the gpu too (an unlit one drew dark for a frame)
static inline byte chunk_mesh_ready(ecs* world, entity chunk, entity mesh) {
    return zox_has(mesh, MeshBuilt) &&
        !zox_has(mesh, BuildMesh) &&
        !zox_has(mesh, MeshDirty) &&
        !zox_has(mesh, MeshColorsDirty) &&
        !zox_chunk_mesh_lighting_busy(world, mesh) &&
        !zox_chunk_lighting_busy(world, chunk);
}

// shows only this mesh: enables it, disables its material's others, links it as the ActiveMesh
static inline void chunk_mesh_show(ecs* world, entity material, entity target) {
    const entity active = zox_get_link(world, material, ActiveMesh);
    if (active != target) {
        if (zox_valid(active)) {
            zox_unlink(world, material, ActiveMesh, active);
        }
        zox_link(world, material, ActiveMesh, target);
    }
    if (zox_has(target, Disabled)) {
        zox_remove(target, Disabled);
    }
    entity meshes[zox_chunk_meshes_max];
    const uint count = zox_get_children_by_id(world, material, meshes, zox_chunk_meshes_max, zox_id(ChunkMesh));
    for (uint i = 0; i < count; i++) {
        if (meshes[i] != target && !zox_has(meshes[i], Disabled)) {
            zox_add(meshes[i], Disabled);
        }
    }
}

// swaps a chunk's material to its mesh at the depth once that's ready: 1 when it's settled
static inline byte chunk_material_transition(ecs* world, entity chunk, entity material, byte depth) {
    const entity target = chunk_mesh_at_depth(world, material, depth);
    const entity preparing = zox_get_link(world, material, PreparingMesh);
    if (preparing != target && zox_valid(preparing)) {
        zox_unlink(world, material, PreparingMesh, preparing);
    }
    // nothing to show at this depth
    if (!target) {
        return 1;
    }
    if (zox_has(target, BuildDisabled)) {
        zox_remove(target, BuildDisabled);
        // merges after this frame, its ready check next frame
        if (preparing != target) {
            zox_link(world, material, PreparingMesh, target);
        }
        return 0;
    }
    const byte ready = chunk_mesh_ready(world, chunk, target);
    if (!zox_has(target, Disabled)) {
        // shown already: once it's ready nothing else should be
        if (ready) {
            if (zox_valid(preparing)) {
                zox_unlink(world, material, PreparingMesh, preparing);
            }
            chunk_mesh_show(world, material, target);
        }
        return ready;
    }
    if (preparing != target) {
        zox_link(world, material, PreparingMesh, target);
    }
    // what's shown stops building while the target builds
    entity meshes[zox_chunk_meshes_max];
    const uint count = zox_get_children_by_id(world, material, meshes, zox_chunk_meshes_max, zox_id(ChunkMesh));
    for (uint i = 0; i < count; i++) {
        const entity mesh = meshes[i];
        if (mesh != target && !zox_has(mesh, Disabled) && !zox_has(mesh, BuildDisabled)) {
            zox_add(mesh, BuildDisabled);
        }
    }
    if (!ready) {
        return 0;
    }
    zox_unlink(world, material, PreparingMesh, target);
    chunk_mesh_show(world, material, target);
    return 1;
}

// swaps a chunk's materials to their meshes at its RenderDepth once they're ready, what they show drawing until then
//  (doc/done/streaming.md), each material on its own
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
        entity materials[zox_block_materials];
        const uint count = get_chunk_materials(world, e, materials);
        byte settled = 1;
        for (uint j = 0; j < count; j++) {
            settled &= chunk_material_transition(world, e, materials[j], depth->value);
        }
        if (settled) {
            zox_remove(e, ChunkLodDirty);
        }
    }
    zox_sys_on_end();
} zoxd_system(chunk_mesh_transition_system);
