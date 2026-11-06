void terrain_callback_inside_block(iter* it) {
    byte dbg_log = 0;
    double spawn_rate = 0.2;
    zox_sys_world();
    zox_sys_begin();
    // cache voxels and colliders for speed
    const BlockLinks* blocks = get_first_terrain_voxels(it);
    if (!blocks) {
        zox_loge("InsideBlockSoundSystem: No BlockLinks");
        return;
    }
    entity sounds[blocks->length];
    get_block_sounds(world, blocks, sounds);
    zox_sys_in(InsideBlock);
    zox_sys_in(Position3D);
    zox_sys_out(InsideBlockTime);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(InsideBlock, inside);
        zox_sys_i(Position3D, position);
        zox_sys_o(InsideBlockTime, time);
        // No sound for air
        if (!inside->value) {
            continue;
        }
        if (zox_current_time - time->value <= spawn_rate) {
            continue;
        }
        // Use block inside data to spawn sound
        // TODO: Block Inside Sounds (for grass)
        entity sound = sounds[inside->value - 1];
        if (!sound) {
            continue;
        }
        // TODO: Link properly to block
        spawn_sound_from_file_name(world, prefab_sound, "hit_grass", 0, get_volume_sfx());
        // position
        entity e2 = spawn_particle3D_emitter_at(
            world,
            position->value,
            3,
            float3_single(0.3f),
            (color) {
                0,
                rand_range(185, 255),
                rand_range(100, 200),
                rand_range(88, 122)
            });
        zox_setv(e2, DestroyInTime, randf_range(0.2f, 0.4f));
        time->value = zox_current_time;
    }
}

