zox_sys2(InsideBlockSoundSystem) {
    double spawn_rate = 0.2;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TerrainLink);
    // cache voxels and colliders for speed
    const BlockLinks* blocks = get_first_terrain_voxels(world, TerrainLink_, it->count);
    if (!blocks) {
        zox_log_error("InsideBlockSoundSystem: No BlockLinks");
        return;
    }
    entity sounds[blocks->length];
    get_block_sounds(world, blocks, sounds);
    zox_sys_in(InsideBlockDirty);
    zox_sys_in(InsideBlock);
    zox_sys_in(Position3D);
    zox_sys_out(InsideBlockTime);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(InsideBlockDirty, idirty);
        zox_sys_i(InsideBlock, inside);
        zox_sys_i(Position3D, position);
        zox_sys_o(InsideBlockTime, time);
        if (idirty->value != zox_dirty_active) {
            continue;
        }
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
        // spawn_sound_generated(world, prefab_sound_generated, instrument_organ, note_frequencies[26 + rand() % 8], 0.4, 1.2f * get_volume_sfx());
        // position
        entity e2 = spawn_particle3D_emitter_at(world, position->value, 3, float3_single(0.3f), (color) { 0, rand_range(185, 255), rand_range(100, 200), rand_range(88, 122) });
        zox_set(e2, DestroyInTime, { randf_range(0.2f, 0.4f) });
        time->value = zox_current_time;
        // zox_sys_e();
        // zox_log("- [%s] inside block [%i] sound [%i]", zox_get_name(e), inside->value, sound);
    }
} zox_sys_end(InsideBlockSoundSystem);
