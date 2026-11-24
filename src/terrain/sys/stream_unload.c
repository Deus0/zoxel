// Unloading streamed out land a little a frame (doc/done/streaming.md): a tunk the stream leaves (its distance past
//  terrain_lod_far) is tagged TunkUnloading (terrain_stream_queue_system), and this deletes them, their chunks with
//  them, up to zox_stream_unload_chunks a frame
//  - deleting a whole row of tunks at once (~585 chunks at far 32, ~15000 commands) made one frame's merge ~11ms
//  - a tunk the stream comes back to before it's gone isn't deleted: its next distance update takes the tag off
//  - tst stream_unload, perf_terrain (its stream's slowest frames)
int zox_stream_unload_chunks = 64;    // chunks deleted a frame at most (0 pauses it: tst stream_unload)

void tunk_unload_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TunkPosition);
    zox_sys_in(Chunk3Stack);
    int deleted = 0;
    for (int i = 0; i < it->count && deleted < zox_stream_unload_chunks; i++) {
        // a tunk's whole stack goes together: a frame can go over by one stack
        zox_sys_e();
        zox_sys_i(TunkPosition, position);
        zox_sys_i(Chunk3Stack, stack);
        const entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain) || !zox_has(terrain, TunkLinks)) {
            zox_delete(e);
            continue;
        }
        zox_muter(terrain, ChunkLinks, chunks);
        for (int k = 0; k < stack->length; k++) {
            const entity chunk = stack->value[k];
            if (!zox_valid(chunk)) {
                continue;
            }
            // The next column now borders the edge of the world. Drop its old faces against this one.
            const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
            for (byte d = 0; d < chunk_neighbors_length; d++) {
                const entity neighbor = neighbors->value[d];
                if (d == direction_up || d == direction_down || !zox_valid(neighbor)) {
                    continue;
                }
                zox_muter(neighbor, ChunkNeighbors, back);
                if (back->value[reverse_direction(d)] == chunk) {
                    back->value[reverse_direction(d)] = 0;
                    zox_add(neighbor, BuildChunkSides);
                }
            }
            int3_hashmap_remove(chunks->value, (int3) { position->value.x, k - stack->length / 2, position->value.y });
            zox_delete(chunk);
            deleted++;
        }
        zox_muter(terrain, TunkLinks, tunks);
        int2_hashmap_remove(tunks->value, position->value);
        zox_delete(e);
    }
    zox_sys_on_end();
} zoxd_system(tunk_unload_system);
