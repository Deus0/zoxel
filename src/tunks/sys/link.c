// Signals Terrain Chunks to Generate -> After Tunk (2D maps) finishes generating
// Terrain Chunk <-> Tunks
zox_sys2(TunkLinkSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Chunk2Position);
    zox_sys_in(VoxLink);
    zox_sys_out(Chunk3Stack);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, generate);
        zox_sys_i(Chunk2Position, cposition);
        zox_sys_i(VoxLink, terrain);
        zox_sys_o(Chunk3Stack, stack);
        zox_geter(terrain->value, ChunkLinks, chunks3);
        for (int j = -render_distance_y, k = 0; j <= render_distance_y; j++, k++) {
            entity chunk3 = stack->value[k];
            if (zox_valid(chunk3)) {
                continue;
            }
            int3 cposition3 = (int3) { cposition->value.x, j, cposition->value.y };
            chunk3 = int3_hashmap_get(chunks3->value, cposition3);
            if (chunk3) {
                stack->value[k] = chunk3;
                zox_set(chunk3, TunkLink, { e });
                if  (!generate->value) {
                    zox_set(chunk3, RenderDepthDirty, { zox_dirty_trigger });
                }
            }
        }
    }
} zox_sys_end(TunkLinkSystem);
