void Characters3DespawnSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDistanceDirty);
    zox_sys_in(CharacterSpawnZone);
    zox_sys_out(CharactersSpawned);
    zox_sys_out(ChunkEntities);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDistanceDirty, state);
        zox_sys_i(CharacterSpawnZone, spawn);
        zox_sys_o(CharactersSpawned, spawned);
        zox_sys_o(ChunkEntities, entities);

        if (state->value != zox_dirty_active) {
            continue;
        }

        // Sometimes characters are in the chunk still when despawning, make sure to check alot!
        if (spawn->value || !entities->length) {
            continue;
        }

        // safety
        byte has_player = 0;
        for (int j = 0; j < entities->length; j++) {
            entity e2 = entities->value[j];
            if (zox_has(e2, PlayerCharacter)) {
                has_player = 1;
                break;
            }
        }
        if (has_player) {
            zox_log_error("trying to despawn chunk npcs with a player in it...!");
            continue;
        }
        zox_logv("- destroying characters [%i] out of [%i]", entities->length, zox_stats_characters);
        for (int j = 0; j < entities->length; j++) {
            entity e2 = entities->value[j];
            zox_delete(e2);
        }
        zox_stats_characters -= entities->length;
        clear_memory_component(ChunkEntities, entities);
        spawned->value = 0;
    }
} zoxd_system2(Characters3DespawnSystem);