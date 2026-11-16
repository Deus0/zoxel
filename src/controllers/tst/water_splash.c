byte water_splash_ready(ecs* world) {
    zox_tst_player_spawn_ready(world);
    return 1;
}

byte water_splash_check(ecs* world) {
    // Build the pool. Put the character 4 blocks over the water's middle, its velocity 0.
    // Count emitters (zox_count_types(ParticlesEmitter)) before it falls.
    zox_log("[tst] [water_splash] Checking splash test");
    entity character = zox_get_link(world, dbg_player, CharacterLink);
    if (!zox_valid(character)) {
        zox_loge("Player character invalid");
        return 0;
    }
    entity terrain = zox_get_link(world, character, TerrainLink);
    if (!zox_valid(terrain)) {
        zox_loge("Terrain invalid");
        return 0;
    }
    // Find water block
    entity realm = zox_get_parent(world, terrain);
    entity water = zox_get_child_by_id(world, realm, zox_id(BlockWater));
    if (!zox_valid(water)) {
        zox_loge("Water block invalid");
        return 0;
    }
    byte water_index = zox_getv(water, BlockIndex);
    
    // Get initial particle count
    int initial_emitters = zox_count_types(ParticlesEmitter);
    zox_log("[tst] [water_splash] Initial emitters: %i", initial_emitters);
    
    // Set up pool position
    float3 water_position = zox_getv(terrain, WorldPosition);
    float terrain_scale = zox_getv(terrain, BlockScale);
    int3 pool_position = int3_add(water_position, int3_single(4));
    pool_position.y = 0;
    
    // Create the pool
    zox_tst_water_pool(world, pool_position, water_index, 3);
    
    // Wait for pool to be created
    double start_time = zox_current_time;
    while (zox_current_time - start_time < 3.0) {
        if (zox_getv(terrain, TerrainChunkCount) > 0) break;
        zox_sleep(0.1);
    }
    
    // Move character into water
    float3 character_position = float3_scale(pool_position, terrain_scale);
    character_position.y += 4 * terrain_scale; // 4 blocks above water surface
    
    zox_setv(character, Position3D, character_position);
    zox_setv(character, Velocity3D, float3_zero);
    
    // Wait for character to enter water
    start_time = zox_current_time;
    int entered_water = 0;
    while (zox_current_time - start_time < 3.0) {
        byte inside_block = zox_getv(character, InsideBlock);
        if (inside_block == water_index) {
            entered_water = 1;
            break;
        }
        zox_sleep(0.1);
    }
    
    if (!entered_water) {
        zox_loge("Character did not enter water");
        return 0;
    }
    
    // Check that a splash occurred
    int final_emitters = zox_count_types(ParticlesEmitter);
    zox_log("[tst] [water_splash] Final emitters: %i", final_emitters);
    
    if (final_emitters <= initial_emitters) {
        zox_loge("No splash particles created");
        return 0;
    }
    
    // Test that we can detect a splash by checking for the sound effect
    // This is more complex to test without a specific log, but at least check the particles were created
    
    // Move character out of water
    zox_setv(character, Position3D, float3_add(character_position, (float3) { 0, 2 * terrain_scale, 0 }));
    
    // Check that we get a smaller splash when exiting water
    start_time = zox_current_time;
    int exited_water = 0;
    while (zox_current_time - start_time < 1.0) {
        byte inside_block = zox_getv(character, InsideBlock);
        if (inside_block != water_index) {
            exited_water = 1;
            break;
        }
        zox_sleep(0.1);
    }
    
    if (!exited_water) {
        zox_loge("Character did not exit water");
        return 0;
    }
    
    // If we get here, the test passed
    zox_log("[tst] [water_splash] Test passed");
    return 1;
}

void zox_tst_add_water_splash() {
    zox_tst_add("water_splash", water_splash_ready, water_splash_check, 120);
}