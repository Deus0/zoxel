// NOTE: Tell the light systems to update when its all done

zox_sys2(SunnyChunkGeneratedSystem) {
    zox_sys_begin();
    zox_sys_in(GenerateChunk);
    zox_sys_out(GenerateLights);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GenerateChunk, generate);
        zox_sys_o(GenerateLights, dirty);
        if (generate->value == zox_generate_tchunk_end) {
            dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(SunnyChunkGeneratedSystem);

zox_sys2(ChunkGeneratedSystem) {
    zox_sys_begin();
    zox_sys_out(GenerateChunk);
    zox_sys_out(VoxelNodeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(GenerateChunk, generate);
        zox_sys_o(VoxelNodeDirty, dirty);
        if (generate->value == zox_generate_tchunk_end) {
            generate->value = 0;
            dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(ChunkGeneratedSystem);
