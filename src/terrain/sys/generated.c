// NOTE: Tell the light systems to update when its all done

zox_sys2(SunnyChunkGeneratedSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(GenerateChunk);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(GenerateChunk, generate);
        if (generate->value == zox_generate_terrain_sunlight) {
            generate->value = zox_generate_terrain_end2;
            zox_add(e, GenerateSunlight);
        } else if (generate->value == zox_generate_terrain_end2) {
            zox_remove(e, GenerateChunk);
            zox_setv(e, VoxelNodeDirty, 1);
            // zox_log("Sunny Generation Compplete [%s]", zox_sys_e_name);
        }
    }
} zox_sys_end(SunnyChunkGeneratedSystem);

zox_sys2(ChunkGeneratedSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(GenerateChunk);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(GenerateChunk, generate);
        if (generate->value == zox_generate_terrain_sunlight) {
            zox_setv(e, VoxelNodeDirty, 1);
            zox_remove(e, GenerateChunk);
            // zox_log("Generation Compplete [%s]", zox_sys_e_name);
        }
    }
} zox_sys_end(ChunkGeneratedSystem);
