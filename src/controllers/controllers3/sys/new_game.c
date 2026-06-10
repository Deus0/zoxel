// NOTE: For new placement of characters, we wait for region/towns to spawn, then we find a position in one of the towns
zox_sys2(PlayerCharacterNewSystem) {
    byte dbg_log = 1;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CameraLink);
    zox_sys_out(PlayerState);
    zox_sys_out(PlayerStateDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(CameraLink, camera);
        zox_sys_o(PlayerState, state);
        zox_sys_o(PlayerStateDirty, dirty);
        if (state->value != zox_player_state_new) {
            continue;
        }
        if (!zox_valid(camera->value)) {
            zox_loge("Invalid Terrain on Realm");
            continue;
        }
        entity game = zox_get_parent(world, e);
        entity realm = zox_getv(game, RealmLink);
        entity terrain = zox_getv(realm, TerrainLink);
        if (!zox_valid(terrain)) {
            zox_loge("Invalid Terrain on Player");
            continue;
        }
        // Wait for Terrain to load
        // Keep active
        /*byte loaded = zox_getv(terrain->value, Loaded);
        if (loaded != zox_load_done) {
            if (dbg_log) {
                zox_log("PlayerCharacterNewSystem: Terrain Loading [%i]", loaded);
            }
            dirty->value = zox_dirty_trigger;
            continue;
        }*/
        // float3 spawn_position = find_new_town_place(world, e, terrain->value, dbg_log);
        int3 town_position = int3_zero;
        entity regions[zox_children_capacity];
        uint regions_length = zox_get_children_by_id(world, terrain, regions, zox_children_capacity, zox_id(Region));
        if (!regions_length) {
            if (dbg_log) {
                zox_log("PlayerCharacterNewSystem: Terrain Loading Regions...");
            }
            continue;
        }
        entity region = regions[rand_range(0, regions_length)];
        entity towns[zox_children_capacity];
        uint towns_length = zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town));
        if (!towns_length) {
            if (dbg_log) {
                zox_log("PlayerCharacterNewSystem: Terrain Loading Towns...");
            }
            continue;
        }
        entity town = towns[rand_range(0, towns_length)];
        int2 town_position2 = zox_getv(town, BlockPosition2);
        town_position.x = town_position2.x;
        town_position.z = town_position2.y;
        float terrain_block_scale = zox_getv(terrain, BlockScale);
        float3 spawn_position = block_position_to_real_position(town_position, terrain_block_scale);
        zox_set(camera->value, Position3D, { spawn_position });
        state->value = zox_player_state_starting;
        dirty->value = zox_dirty_trigger;
        if (dbg_log) {
            zox_log("Player Now State: [Starting]");
            zox_log("   - Town [%s] Position [%ix%i]", zox_get_name(town), town_position2.x, town_position2.y);
            zox_log("   - Player Spawn Position [%fx%fx%f]", spawn_position.x, spawn_position.y, spawn_position.z);
        }
    }
} zox_sys_end(PlayerCharacterNewSystem);
