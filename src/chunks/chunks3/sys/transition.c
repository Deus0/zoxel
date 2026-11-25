extern byte zox_chunk_lighting_busy(ecs*, entity);
extern byte zox_chunk_mesh_lighting_busy(ecs*, entity);

// a chunk relighting or generating (its voxels, or again finer for a lod: ChunkLodDirty generate, generating): its
//  shown meshes hold (BuildDisabled), relit at their old depth they flickered dark, meshed from half made voxels they
//  showed what wasn't there
static inline byte chunk_mesh_held(ecs* world, entity chunk) {
    return zox_chunk_lighting_busy(world, chunk) || (zox_has(chunk, GenerateChunk) && zox_getv(chunk, GenerateChunk)) ||
        (zox_has(chunk, ChunkLodDirty) && zox_getv(chunk, ChunkLodDirty) == zox_chunk_lod_dirty_generating);
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

// a neighbour's faces against this chunk still to build: a shown mesh's mesh, one held mid swap of its
//  own too (BuildDisabled builds once its chunk's relit, that never waits on a swap: two side by side don't lock)
static inline byte chunk_neighbors_rebuilding(ecs* world, entity chunk) {
    const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
    for (byte n = 0; n < chunk_neighbors_length; n++) {
        const entity neighbor = neighbors->value[n];
        if (!zox_valid(neighbor) || !zox_has(neighbor, Chunk3)) {
            continue;
        }
        entity meshes[zox_chunk_meshes_max];
        const uint count = get_chunk_meshes(world, neighbor, meshes);
        for (uint i = 0; i < count; i++) {
            const entity mesh = meshes[i];
            if (!zox_has(mesh, Disabled) && (zox_has(mesh, BuildMesh) || zox_has(mesh, MeshDirty))) {
                return 1;
            }
        }
    }
    return 0;
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
static inline byte chunk_material_transition(ecs* world, entity chunk, entity material, byte depth, byte neighbors_busy) {
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
    // and its neighbours' faces against it built: a swap before showed holes where they hadn't caught up
    const byte ready = chunk_mesh_ready(world, chunk, target) && !neighbors_busy;
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
    // what's shown stops building while its chunk relights or generates (relit at their old depth they flickered
    //  dark), chunk_mesh_hold_system lets it build again after
    if (chunk_mesh_held(world, chunk)) {
        entity meshes[zox_chunk_meshes_max];
        const uint count = zox_get_children_by_id(world, material, meshes, zox_chunk_meshes_max, zox_id(ChunkMesh));
        for (uint i = 0; i < count; i++) {
            const entity mesh = meshes[i];
            if (mesh != target && !zox_has(mesh, Disabled) && !zox_has(mesh, BuildDisabled)) {
                zox_add(mesh, BuildDisabled);
            }
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
//    stop building while the chunk relights (relit at their old depth they flickered dark), then it swaps in one step
//    once chunk_mesh_ready
//  - swaps once its neighbours' faces against it are built (chunk_neighbors_rebuilding): open into it while it's
//    unsettled (chunk_side_depth), rebuilt against what it draws once it's settled
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
        const byte neighbors_busy = chunk_neighbors_rebuilding(world, e);
        byte settled = 1;
        for (uint j = 0; j < count; j++) {
            settled &= chunk_material_transition(world, e, materials[j], depth->value, neighbors_busy);
        }
        if (settled) {
            zox_remove(e, ChunkLodDirty);
            // settled: its neighbours' faces follow what it draws now (open into it while it swapped)
            chunk_neighbors_rebuild_sides(world, e);
        }
    }
    zox_sys_on_end();
} zoxd_system(chunk_mesh_transition_system);

// NOTE: Lets a shown chunk mesh held (BuildDisabled: a lod swap stopped it building) build again once its chunk has
//  relit and generated: a neighbour's lod or voxels changed its faces, held stale beside them it was a hole for as
//  long as its swap took (tst chunk_mesh_flicker); its own system, as the swap skips chunks while their sides build
void chunk_mesh_hold_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity material = zox_get_parent(world, e);
        const entity chunk = zox_valid(material) ? zox_get_parent(world, material) : 0;
        if (zox_valid(chunk) && !chunk_mesh_held(world, chunk)) {
            zox_remove(e, BuildDisabled);
        }
    }
    zox_sys_on_end();
} zoxd_system(chunk_mesh_hold_system);
