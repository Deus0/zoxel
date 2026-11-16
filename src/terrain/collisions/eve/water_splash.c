void water_splash_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    // get water block
    entity terrain = zox_get_link(world, e, TerrainLink);
    if (!zox_valid(terrain)) {
        return;
    }
    entity realm = zox_get_parent(world, terrain);
    entity water = zox_get_child_by_id(world, realm, zox_id(BlockWater));
    if (!zox_valid(water)) {
        return;
    }
    byte water_index = zox_getv(water, BlockIndex);
    // get components
    zox_sys_in(InsideBlock);
    zox_sys_in(Position3D);
    zox_sys_in(Velocity3D);
    zox_sys_out(InsideBlockLast);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(InsideBlock, inside);
        zox_sys_i(Position3D, position);
        zox_sys_i(Velocity3D, velocity);
        zox_sys_o(InsideBlockLast, last_inside);
        // get last block
        byte last_block = last_inside->value;
        // entering water (wasn't water, now is water, and falling fast)
        if (last_block != water_index && inside->value == water_index) {
            if (velocity->value.y < -2) {
                // calculate splash position at water surface
                int3 block_position = zox_getv(e, InsideBlockPosition);
                float3 splash_position = float3_scale((float3) { (float) block_position.x, (float) block_position.y, (float) block_position.z }, zox_getv(terrain, BlockScale));
                splash_position.y += zox_getv(terrain, BlockScale);
                // spawn splash
                entity emitter = spawn_particle3D_emitter_at(world, splash_position, 12, (float3) { 0.6f, 0.2f, 0.6f }, (color) { 120, 170, 240, 200 });
                zox_setv(emitter, DestroyInTime, 0.3f);
                spawn_sound_effect_at(world, sound_effect_splash, splash_position);
            }
        }
        // leaving water (was water, now isn't)
        else if (last_block == water_index && inside->value != water_index) {
            // calculate splash position at water surface
            int3 block_position = zox_getv(e, InsideBlockPosition);
            float3 splash_position = float3_scale((float3) { (float) block_position.x, (float) block_position.y, (float) block_position.z }, zox_getv(terrain, BlockScale));
            splash_position.y += zox_getv(terrain, BlockScale);
            // spawn small splash
            entity emitter = spawn_particle3D_emitter_at(world, splash_position, 6, (float3) { 0.6f, 0.2f, 0.6f }, (color) { 120, 170, 240, 100 });
            zox_setv(emitter, DestroyInTime, 0.3f);
            spawn_sound_effect_at(world, sound_effect_splash, splash_position);
        }
        // update last block
        last_inside->value = inside->value;
    }
}