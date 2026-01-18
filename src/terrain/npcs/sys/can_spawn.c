zox_sys2(Characters3SpawnZoneSystem) {
    zox_sys_begin();
    zox_sys_in(RenderDistanceDirty);
    zox_sys_in(RenderDistance);
    zox_sys_out(CharacterSpawnZone);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDistanceDirty, state);
        zox_sys_i(RenderDistance, distance);
        zox_sys_o(CharacterSpawnZone, spawn);

        // update when dirty
        if (state->value == zox_dirty_active) {
            spawn->value = distance->value <= terrain_lod_near;
        }
    }
} zox_sys_end(Characters3SpawnZoneSystem);