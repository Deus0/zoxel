// TODO: Fix this check, atm im using a hacky counter to fix loading checks
//  Solution:
//          - everytime a chunk spawns, add to terrain count
//          - everytime it finishes, decrease
//  A simple way to make sure its done

// A state checker for stream loading
zox_sys2(StreamEndSystem) {
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
                entity active_mesh = zox_getv(e2, ActiveMesh);
                // zox_get_link(world, e2, ActiveMesh);
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
        if (chunks_loading > 0) {
            running = 1;
        }
        if (running && !zox_disable) {
            continue;
        }
        // NOTE: Can we get render distance from the chunks here?
        uint chunk_required = terrain_lod_far * terrain_lod_far * (render_distance_y) * required_buffer;
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
} zox_sys_end(StreamEndSystem);
