// TODO: Fix this check, atm im using a hacky counter to fix loading checks
//  Solution:
//          - everytime a chunk spawns, add to terrain count
//          - everytime it finishes, decrease
//  A simple way to make sure its done

// A state checker for stream loading
zox_sys2(StreamEndEventSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(EventInput);
    zox_sys_in(ChunkLinks);
    zox_sys_out(StreamEndEvent);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(StreamEndEvent, event);
        zox_sys_i(ChunkLinks, chunks);
        zox_sys_i(EventInput, eventInput);

        if (!event->value) {
            continue;
        }

        if (!chunks->value || !chunks->value->size) {
            continue;
        }

        // check all chunks chunks if chunks are dirty]
        uint chunks_loaded = 0;
        byte running = 0;
        for (size_t j = 0; j < chunks->value->size; j++) {
            int3_hashmap_pair* pair = chunks->value->data[j];

            uint checks = 0;
            while (pair != NULL && checks < max_safety_checks_hashmap) {
                entity chunk = pair->value;

                if (!zox_valid(chunk) || !zox_has(chunk, GenerateChunk) || !zox_has(chunk, ChunkMeshDirty)) {
                    if (!zox_valid(chunk)) {
                        zox_log_error("chunk invalid in stream end system [%lu]", chunk);
                    } else if (!zox_has(chunk, GenerateChunk)) {
                        zox_log_error("chunk has no GenerateChunk [%lu]", chunk);
                    } else if (!zox_has(chunk, ChunkMeshDirty)) {
                        zox_log_error("chunk has no ChunkMeshDirty [%lu]", chunk);
                    } else {
                        zox_log_error("chunk invalid not sure why[%lu]", chunk);
                    }
                    running = 1;
                } else if (zox_gett_value(chunk, RenderDepth) == render_depth_spawning) {
                    running = 1;
                } else if (zox_gett_value(chunk, ChunkMeshDirty)) {
                    running = 1;
                } else if (zox_gett_value(chunk, GenerateChunk)) {
                    running = 1;
                }

                if (running) {
                    break;
                }

                int3_hashmap_pair* next_pair = pair->next;
                pair = next_pair;
                chunks_loaded++;
                checks++;
            }
            if (running) {
                break;
            }
        }

        // also checks if loaded enough chunks
        int xz_chunks = terrain_lod_near * 2 + 1;
        int y_chunks = render_distance_y * 2 + 1;
        uint chunk_required = xz_chunks * xz_chunks * y_chunks;

        if (!running && chunks_loaded >= chunk_required) {
            zox_log("Terrain Loaded: @ [%f]\n   - chunks: [%i]", zox_current_time, chunks_loaded);

            // we should check if all chunks have finished here
            (*event->value)(world, eventInput->value);
            event->value = NULL;
        }

    }
} zox_sys_end(StreamEndEventSystem);