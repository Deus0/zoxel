// generate terrain after Tunk generates
zox_sys2(TunkEndSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Chunk2Position);
    zox_sys_in(VoxLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(Chunk2Position, cposition);
        zox_sys_i(VoxLink, terrain);

        if (generate->value != zox_dirty_end) {
            continue;
        }

        zox_geter(terrain->value, ChunkLinks, chunks3);

        for (int j = -4; j <= 4; j++) {
            int3 cposition3 = (int3) { cposition->value.x, j, cposition->value.y };
            entity chunk3 = int3_hashmap_get(chunks3->value, cposition3);
            if (chunk3) {
                zox_set(chunk3, RenderDepthDirty, { zox_dirty_trigger });
            }
        }

    }
} zox_sys_end(TunkEndSystem);