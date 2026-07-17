// NOTE: For new placement of characters, we wait for region/towns to spawn, then we find a position in one of the towns
zox_sys2(PlayerTownFinderSystem) {
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
#ifdef zox_safety_checks
        if (!zox_valid(camera->value)) {
            zox_loge("Invalid Terrain on Realm");
            continue;
        }
#endif
        entity game = zox_get_parent(world, e);
#ifdef zox_safety_checks
        if (!zox_valid(game)) {
            zox_loge("Invalid game on Player");
            continue;
        }
#endif
        entity realm = zox_getv(game, RealmLink);
#ifdef zox_safety_checks
        if (!zox_valid(realm)) {
            zox_loge("Invalid realm on Player");
            continue;
        }
#endif
        // TODO: Just make a Realm Generating flag
        byte realm_busy = 0;
        entity textures[zox_children_capacity];
        uint textures_length = zox_get_children_by_id(world, realm, textures, zox_children_capacity, zox_id(Texture));
        for (int j = 0; j < textures_length; j++) {
            entity texture = textures[j];
            if (zox_has(texture, Busy) && zox_getv(texture, Busy)) {
                if (dbg_log) {
                    zox_log("Realm texture Still Loading [%s]", zox_get_name(texture));
                }
                realm_busy = 1;
                break;
            }
        }
        if (realm_busy) {
            if (dbg_log) {
                zox_log("Realm Still Loading");
            }
            continue;
        }
        entity terrain = zox_getv(realm, TerrainLink);
#ifdef zox_safety_checks
        if (!zox_valid(terrain)) {
            zox_loge("Invalid Terrain on Player");
            continue;
        }
#endif
        entity regions[zox_children_capacity];
        uint regions_length = zox_get_children_by_id(world, terrain, regions, zox_children_capacity, zox_id(Region));
        if (!regions_length) {
            if (dbg_log) {
                zox_log("PlayerTownFinderSystem: Terrain Loading Regions...");
            }
            continue;
        }
        entity region = regions[rand_range(0, regions_length)];
#ifdef zox_safety_checks
        if (!zox_valid(region)) {
            continue;
        }
        if (!zox_has(region, GenerateRegion)) {
            zox_loge("Region Missing GenerateRegion");
            continue;
        }
#endif
        if (zox_getv(region, GenerateRegion)) {
            if (dbg_log) {
                zox_log("Terrain Loading Regions... (PlayerTownFinderSystem)");
            }
            continue;
        }
        int3 town_position = int3_zero;
        town_position.y = 8;
        entity town = 0;
        entity towns[zox_children_capacity];
        uint towns_length = zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town));
        if (towns_length) {
            town = towns[rand_range(0, towns_length)];
            int2 town_position2 = zox_getv(town, BlockPosition2);
            town_position.x = town_position2.x;
            town_position.z = town_position2.y;
        } else {
            if (dbg_log) {
                zox_log("PlayerTownFinderSystem: Terrain Region [%s] has no Towns", zox_get_name(region));
            }
        }
        float terrain_block_scale = zox_getv(terrain, BlockScale);
        float3 spawn_position = block_position_to_real_position(town_position, terrain_block_scale);
        // NOTE: Make sure it updates even if position the same
        state->value = zox_player_state_starting;
        dirty->value = zox_dirty_trigger;
        zox_set(camera->value, Position3D, { spawn_position });
        zox_set(camera->value, StreamDirty, { zox_dirty_trigger });
        zox_set(camera->value, StreamerLevel, { 1 });
        // zox_set(terrain, Loaded, { zox_load_begin });
        if (dbg_log) {
            byte2 town_size2 = town ? zox_getv(town, TownSize) : byte2_zero;
            zox_log("Player Now State: [Starting]");
            zox_log("   - Town [%s] Position [%ix%i] Size [%ix%i]", zox_get_name(town), town_position.x, town_position.z, town_size2.x, town_size2.y);
            zox_log("   - Player Spawn Position [%fx%fx%f]", spawn_position.x, spawn_position.y, spawn_position.z);
        }
    }
} zox_sys_end(PlayerTownFinderSystem);
