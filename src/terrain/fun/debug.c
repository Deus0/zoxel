int get_terrain_chunks_count(ecs *world) {
    return zox_count_types(TerrainChunk)
}

entity test_chunk_terrain = 0;

// h
void test_spawn_chunk_terrain(ecs *world, int32_t keycode) {
    if (keycode == zox_key_h) {
        if (test_chunk_terrain) {
            zox_log("- deleting test chunk terrain");
            zox_delete(test_chunk_terrain);
            test_chunk_terrain = 0;
        } else {
            zox_log("+ spawning test chunk terrain");
            // spawn at here
            int3 chunk_position = int3_zero;

            entity terrain = local_terrain;

            if (!zox_valid(terrain)) return;

            zox_geter_value(terrain, BlockScale, float, terrain_scale);
            test_chunk_terrain = spawn_chunk_terrain(
                world,
                prefab_chunk_terrain,
                terrain,
                chunk_position,
                int3_zero,
                terrain_depth,
                terrain_scale
            );
        }
    }
}
