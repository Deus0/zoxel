// the town new players spawn in, the first player's (local coop, tst coop_homes)
entity player_spawn_town = 0;

// NOTE: For new placement of characters, we wait for region/towns to spawn, then we find a position in one of the towns
void player_town_finder_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(PlayerState);
    zox_sys_out(PlayerStateDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(PlayerState, state);
        zox_sys_o(PlayerStateDirty, dirty);
        if (state->value != zox_player_state_new) {
            continue;
        }
        entity camera = zox_get_link(world, e, CameraLink);
        if (!zox_valid(camera)) {
            zox_loge("Invalid Camera on Player");
            continue;
        }
        entity game = zox_get_parent(world, e);
#ifdef zox_safety_checks
        if (!zox_valid(game)) {
            zox_loge("Invalid game on Player");
            continue;
        }
#endif
        entity realm = zox_get_link(world, game, RealmLink);
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
            if (zox_has(texture, GenerateTexture) && zox_getv(texture, GenerateTexture)) {
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
        entity terrain = zox_get_link(world, realm, TerrainLink);
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
                zox_log("player_town_finder_system: Terrain Loading Regions...");
            }
            continue;
        }
        // local coop: every player in the first one's town, each at their own home
        //  - the others wait for the first to pick it (players run on other threads, they picked their own)
        const byte player_index = get_player_index(e);
        if (player_index && zox_valid(zox_players[0]) && zox_getv(zox_players[0], PlayerState) == zox_player_state_new) {
            continue;
        }
        entity town = player_spawn_town;
        if (!zox_valid(town) || zox_get_parent(world, zox_get_parent(world, town)) != terrain) {
            // a town of any region's, once they've all generated (a random region often had none)
            byte regions_busy = 0;
            entity towns[zox_children_capacity];
            uint towns_length = 0;
            for (uint j = 0; j < regions_length && !regions_busy; j++) {
                const entity region = regions[j];
                if (!zox_valid(region) || !zox_has(region, GenerateRegion) || zox_getv(region, GenerateRegion)) {
                    regions_busy = 1;
                    break;
                }
                towns_length += zox_get_children_by_id(world, region, towns + towns_length,
                    zox_children_capacity - towns_length, zox_id(Town));
            }
            if (regions_busy) {
                continue;
            }
            town = towns_length ? towns[rand_range(0, towns_length)] : 0;
            player_spawn_town = town;
        }
        // its homes spawn a frame after it
        if (town && zox_has(town, GenerateTown) && zox_getv(town, GenerateTown)) {
            continue;
        }
        // no town: side by side (they spawn on their own column)
        int3 town_position = (int3) { player_index * 3, 8, 0 };
        if (town) {
            const int2 town_position2 = zox_getv(town, BlockPosition2);
            town_position.x = town_position2.x;
            town_position.z = town_position2.y;
            entity homes[zox_children_capacity];
            const uint homes_length = zox_get_children_by_id(world, town, homes, zox_children_capacity, zox_id(Home));
            const entity home = homes_length ? homes[player_index % homes_length] : 0;
            if (home) {
                const int2 door = home_door_front(world, home);
                town_position.x = door.x;
                town_position.z = door.y;
            }
        }
        float terrain_block_scale = zox_getv(terrain, BlockScale);
        float3 spawn_position = block_position_to_real_position(town_position, terrain_block_scale);
        // NOTE: Make sure it updates even if position the same
        state->value = zox_player_state_starting;
        dirty->value = zox_dirty_trigger;
        zox_setv(camera, Position3D, spawn_position);
        zox_setv(camera, StreamDirty, zox_dirty_trigger);
        zox_setv(camera, StreamerLevel, 1);
        // Clear spawn queue (the other players' streamers queue theirs again, their land went missing)
        zox_muter(terrain, TerrainSpawnQueue, queue);
        clear_TerrainSpawnQueue(queue);
        for (int j = 0; j < players_playing; j++) {
            const entity other = zox_get_link(world, zox_players[j], CameraLink);
            if (zox_valid(other) && other != camera && zox_has(other, StreamDirty)) {
                zox_setv(other, StreamDirty, zox_dirty_trigger);
            }
        }
        // zox_set(terrain, Loaded, { zox_load_begin });
        if (dbg_log) {
            byte2 town_size2 = town ? zox_getv(town, TownSize) : byte2_zero;
            zox_log("Player Now State: [Starting]");
            zox_log("   - Town [%s] Position [%ix%i] Size [%ix%i]",
                zox_get_name(town),
                town_position.x,
                town_position.z,
                town_size2.x,
                town_size2.y);
            zox_log("   - Player Spawn Position [%fx%fx%f]",
                spawn_position.x,
                spawn_position.y,
                spawn_position.z);
        }
    }
    zox_sys_on_end();
} zoxd_system(player_town_finder_system);
