// TODO: Fix this check, atm im using a hacky counter to fix loading checks
//  Solution:
//          - everytime a chunk spawns, add to terrain count
//          - everytime it finishes, decrease
//  A simple way to make sure its done

// A state checker for stream loading
zox_sys2(StreamEndSystem) {
    byte dbg_log = 1;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(EventInput);
    zox_sys_in(ChunkLinks);
    zox_sys_out(Loaded);
    zox_sys_out(StreamEndEvent);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChunkLinks, chunks);
        zox_sys_i(EventInput, input);
        zox_sys_o(Loaded, loaded);
        zox_sys_o(StreamEndEvent, event);
        if (loaded->value != zox_load_begin) {
            continue;
        }
        if (!chunks->value || !chunks->value->size) {
            continue;
        }
        uint chunks_loaded = 0;
        byte running = 0;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count && !running; j++) {
                entity e2 = it2.entities[j];
                if (!zox_has(e2, Chunk3)) {
                    continue;
                }
                if (zox_getv(e2, Busy)) {
                    running = 1;
                    break;
                }
                chunks_loaded++;
            }
        }
        // NOTE: Can we get render distance from the chunks here?
        uint chunk_required = terrain_lod_far * terrain_lod_far;
        if (!running && chunks_loaded >= chunk_required) {
            // we should check if all chunks have finished here
            if (event->value) {
                (*event->value)(world, input->value);
                event->value = NULL;
            }
            // now loaded
            loaded->value = zox_load_done;
            if (dbg_log) {
                zox_log("Terrain Loaded: chunks: [%i]", chunks_loaded);
            }
        }
    }
} zox_sys_end(StreamEndSystem);


        /*for (size_t j = 0; j < chunks->value->size; j++) {
            int3_hashmap_pair* pair = chunks->value->data[j];
            uint checks = 0;
            while (pair != NULL && checks < max_safety_checks_hashmap) {
                entity chunk = pair->value;*/
                /*if (!zox_valid(e2) || !zox_has(e2, Generate) || !zox_has(e2, ChunkMeshDirty)) {
                    if (!zox_valid(chunk)) {
                        zox_log_error("chunk invalid in stream end system [%lu]", chunk);
                    } else if (!zox_has(e2, Generate)) {
                        zox_log_error("chunk has no Generate [%lu]", e2);
                    } else if (!zox_has(e2, ChunkMeshDirty)) {
                        zox_log_error("chunk has no ChunkMeshDirty [%lu]", chunk);
                    } else {
                        zox_log_error("chunk invalid not sure why[%lu]", chunk);
                    }
                    running = 1;
                }*/
                /* else if (zox_getv(chunk, RenderDepth) == render_depth_uninitialized) {
                    running = 1;
                } */
                /*int3_hashmap_pair* next_pair = pair->next;
                pair = next_pair;
                chunks_loaded++;
                checks++;
            }
            if (running) {
                break;
            }*/
