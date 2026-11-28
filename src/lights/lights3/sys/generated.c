// NOTE: Tell the light systems to update when its all done

void sunny_chunk_generated_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(GenerateChunk);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(GenerateChunk, generate);
        if (!zox_lighting && generate->value == zox_generate_terrain_sunlight) {
            zox_remove(e, GenerateChunk);
            zox_add(e, VoxelNodeDirty);
        } else if (generate->value == zox_generate_terrain_sunlight) {
            generate->value = zox_generate_terrain_end2;
            zox_add(e, GenerateSunlight);
            zox_add(e, GenerateEmitterLight);
            // its neighbours' light into it (lights3/fun/relight.c)
            chunk_relight_from_neighbors(world, e);
        } else if (generate->value == zox_generate_terrain_end2) {
            zox_remove(e, GenerateChunk);
            zox_add(e, VoxelNodeDirty);
            // zox_log("Sunny Generation Compplete [%s]", zox_sys_e_name);
        }
    }
    zox_sys_on_end();
} zoxd_system(sunny_chunk_generated_system);

// NOTE Our chunk generation ends here
void chunk_generated_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(GenerateChunk);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(GenerateChunk, generate);
        if (generate->value == zox_generate_terrain_sunlight) {
            if (zox_lighting) {
                zox_add(e, GenerateEmitterLight);
                // its neighbours' light into it: a sunbeam from above, floods from the sides (lights3/fun/relight.c);
                //  regenerated for a lod under an air chunk that wasn't, it stayed dark
                chunk_relight_from_neighbors(world, e);
            }
            zox_add(e, VoxelNodeDirty);
            zox_remove(e, GenerateChunk);
            // zox_log("Generation Compplete [%s]", zox_sys_e_name);
        }
    }
    zox_sys_on_end();
} zoxd_system(chunk_generated_system);
