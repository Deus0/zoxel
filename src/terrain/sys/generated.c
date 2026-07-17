// NOTE: Tell the light systems to update when its all done

zox_sys2(SunnyChunkGeneratedSystem) {
    zox_sys_begin();
    zox_sys_in(GenerateChunk);
    zox_sys_out(GenerateLights);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GenerateChunk, generate_chunk);
        zox_sys_o(GenerateLights, generate_lights);
        if (generate_chunk->value == zox_generate_terrain_end) {
            generate_lights->value = zox_generate_lights_start;
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
        if (generate->value == zox_generate_terrain_end) {
            generate->value = 0;
            dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(ChunkGeneratedSystem);
