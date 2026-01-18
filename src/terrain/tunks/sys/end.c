// Signals Terrain Chunks to Generate -> After Tunk (2D maps) finishes generating
zox_sys2(TunkEndSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Chunk3Stack);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, generate);
        zox_sys_i(Chunk3Stack, stack);

        if (generate->value != zox_dirty_end) {
            continue;
        }

        for (int j = -render_distance_y, k = 0; j <= render_distance_y; j++, k++) {
            entity chunk3 = stack->value[k];

            if (!zox_valid(chunk3)) {
                continue;
            }

            zox_set(chunk3, RenderDepthDirty, { zox_dirty_trigger });
        }
    }
} zox_sys_end(TunkEndSystem);