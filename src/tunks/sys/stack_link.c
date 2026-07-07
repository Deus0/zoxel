// Signals Terrain Chunks to Generate -> After Tunk (2D maps) finishes generating
// Terrain Chunk <-> Tunks
// NOTE: Links Tunks to Chunks
zox_sys2(TunkLinkSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(TunkPosition);
    zox_sys_out(Chunk3Stack);
    zox_sys_out(Chunk3Stacked);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, generate);
        zox_sys_i(TunkPosition, cposition);
        zox_sys_o(Chunk3Stack, stack);
        zox_sys_o(Chunk3Stacked, stacked);
        /*if (stacked->value) {
            continue;
        }*/
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
            continue;
        }
        byte any_invalid = 0;
        const ChunkLinks* chunks3 = zox_get(terrain, ChunkLinks);
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
            if (!zox_valid(chunk3)) {
                any_invalid = 1;
            }
        }
        if (!any_invalid) {
            stacked->value = 1;
        }
    }
} zox_sys_end(TunkLinkSystem);
