// why terrain is still Loading, for tests/debugging
char zox_stream_end_status[160] = "";

// chunks streaming keeps within terrain_lod_near, a circle of columns (int2_distance, like terrain_stream_system)
//  - used to be the full square, its 4 corners never spawn, so Loading never ended for some positions
static inline uint get_stream_near_chunks_count() {
    uint columns = 0;
    for (int x = -terrain_lod_near; x <= terrain_lod_near; x++) {
        for (int z = -terrain_lod_near; z <= terrain_lod_near; z++) {
            columns += int2_distance(int2_zero, (int2) { x, z }) <= terrain_lod_near;
        }
    }
    return columns * (2 * render_distance_y + 1);
}

// A state checker for stream loading
// Loading ends once the chunks near the streamer are done (RenderDistance <= terrain_lod_near),
// the far ones keep streaming in after (spawn queue is sorted by distance, closest first)
void StreamEndSystem(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    byte zox_disable = 0;
    float required_buffer = 0.95f;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkLinks);
    zox_sys_in(TerrainSpawnQueue);
    zox_sys_in(StreamEndEvent);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChunkLinks, chunks);
        zox_sys_i(TerrainSpawnQueue, queue);
        zox_sys_i(StreamEndEvent, event);
        if (!chunks->value || !chunks->value->size) {
            continue;
        }
        uint chunks_loaded = 0;
        uint chunks_loading = 0;
        uint chunks_generating = 0;
        uint chunks_busy = 0;
        uint meshes_loading = 0;
        byte lowest_generate_state = 255;
        byte running = 0;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                if (!zox_has(e2, Chunk3)) {
                    continue;
                }
                if (!zox_has(e2, RenderDistance) || zox_getv(e2, RenderDistance) > terrain_lod_near) {
                    continue;
                }
                if (zox_has(e2, GenerateChunk)) {
                    byte state = zox_getv(e2, GenerateChunk);
                    if (state < lowest_generate_state) {
                        lowest_generate_state = state;
                    }
                    chunks_loading++;
                    chunks_generating++;
                    continue;
                }
                if (zox_has(e2, BuildChunkSides) ||
                    // zox_has(e2, VoxelNodeDirty) ||
                    zox_has(e2, Initialize))
                {
                    chunks_loading++;
                    chunks_busy++;
                    continue;
                }
                // byte chunk_mesh_loading = 0;
                // entity active_mesh = zox_getv(e2, ActiveMesh);
                entity active_mesh = zox_get_link(world, e2, ActiveMesh);
                byte chunk_mesh_loading =
                    zox_valid(active_mesh) &&
                        (zox_has(active_mesh, BuildMesh) ||
                        zox_has(active_mesh, MeshDirty));
                /*iter it3 = zox_children(world, e2);
                while (zox_children_next(it3) && !chunk_mesh_loading) {
                    for (int k = 0; k < it3.count && !chunk_mesh_loading; k++) {
                        entity e3 = it3.entities[k];
                        if (!zox_has(e3, ChunkMesh)) {
                            continue;
                        }
                        if (zox_has(e3, BuildMesh) ||
                            zox_has(e3, MeshDirty))
                        {
                            chunk_mesh_loading = 1;
                        }
                    }
                }*/
                if (chunk_mesh_loading) {
                    meshes_loading++;
                    chunks_loading++;
                    continue;
                }
                chunks_loaded++;
            }
        }
        if (dbg_log) {
            zox_log("[Loading Screen]\n - Queued [%i]\n - Loading [%i] of [%i] - State [%i]\n - Generating [%i]\n - Busy [%i]\n - Meshes [%i]",
                queue->count,
                chunks_loading,
                chunks_loading + chunks_loaded,
                lowest_generate_state,
                chunks_generating,
                chunks_busy,
                meshes_loading);
        }
        const uint near_count = get_stream_near_chunks_count();
        snprintf(zox_stream_end_status, sizeof(zox_stream_end_status),
            "near loaded [%u/%u] loading [%u] generating [%u] (lowest [%i]) busy [%u] meshes [%u] closest queued [%i]",
            chunks_loaded,
            near_count,
            chunks_loading, chunks_generating, lowest_generate_state, chunks_busy, meshes_loading,
            get_closest_TerrainSpawnQueue_distance((TerrainSpawnQueue*) queue));
        if (chunks_loading > 0) {
            running = 1;
        }
        // near columns still waiting in the spawn queue
        if (get_closest_TerrainSpawnQueue_distance((TerrainSpawnQueue*) queue) <= terrain_lod_near) {
            running = 1;
        }
        if (running && !zox_disable) {
            continue;
        }
        uint chunk_required = near_count * required_buffer;
        if (chunks_loaded < chunk_required && !zox_disable) {
            if (dbg_log) {
                zox_log("Chunks Didnt load enough: [%i] < [%i]", chunks_loaded, chunk_required);
            }
            continue;
        }
        zox_remove(e, Loading);
        // we should check if all chunks have finished here
        if (event->value) {
            (*event->value)(world, e);
        }
        if (dbg_log) {
            zox_log("Terrain Loaded: chunks: [%i]", chunks_loaded);
        }
    }
    zox_sys_on_end();
} zoxd_system(StreamEndSystem);
