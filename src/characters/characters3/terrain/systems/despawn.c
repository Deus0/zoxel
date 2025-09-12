void Characters3DespawnSystem(iter *it) {
    zox_sys_world()
    zox_sys_begin()
    zox_sys_in(RenderDistanceDirty)
    zox_sys_in(CharacterSpawnZone)
    zox_sys_out(CharactersSpawned)
    zox_sys_out(ChunkEntities)
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDistanceDirty, state)
        zox_sys_i(CharacterSpawnZone, spawn)
        zox_sys_o(CharactersSpawned, spawned)
        zox_sys_o(ChunkEntities, entities)
        if (state->value != zox_dirty_active) {
            continue;
        }

        // Sometimes characters are in the chunk still when despawning, make sure to check alot!
        if (!spawn->value) {
            // safety
            byte has_player = 0;
            for (int j = 0; j < entities->length; j++) {
                if (zox_has(entities->value[j], PlayerCharacter)) {
                    has_player = 1;
                    break;
                }
            }
            if (has_player) {
                zox_log_error("trying to despawn chunk npcs with a player in it...!");
                continue;
            }
            // zox_log("- destroying characters [%i] out of [%i]", entities->length, zox_stats_characters)
            for (int j = 0; j < entities->length; j++) {
                zox_delete(entities->value[j]);
            }
            zox_stats_characters -= entities->length;
            clear_memory_component(ChunkEntities, entities)
            spawned->value = 0;
        }
    }
} zoxd_system(Characters3DespawnSystem)