zox_sys2(Characters3SpawnZoneSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Loaded);
    zox_sys_in(RenderDepth);
    zox_sys_in(RenderDistance);
    zox_sys_out(NpcSpawnZone);
    zox_sys_out(NpcSpawnZoneDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, generate);
        zox_sys_i(Loaded, loaded);
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(RenderDistance, distance);
        zox_sys_o(NpcSpawnZone, active);
        zox_sys_o(NpcSpawnZoneDirty, dirty);
        // or loading
        if (generate->value) {
            active->value = 0;
            continue;
        }
        if (dirty->value) {
            continue;
        }
        // Max Depth Checks
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
            zox_loge("Terrain missing on chunk3_terrain");
            continue;
        }
        float terrain_depth = zox_getv(terrain, NodeDepth);
        if (depth->value != terrain_depth) {
            active->value = 0;
            continue;
        }
        active->value = distance->value <= terrain_lod_near;
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(Characters3SpawnZoneSystem);
