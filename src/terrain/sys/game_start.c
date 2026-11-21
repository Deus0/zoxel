extern const double game_spawn_terrain_delay;

void game_state_terrain(ecs* world, entity game, byte state) {
    if (state != zox_game_terrain_spawn) {
        return;
    }
    entity realm = zox_get_link(world, game, RealmLink);
    if (!zox_valid(realm)) {
        zox_loge("[game_state_terrain] Invalid [realm]");
        return;
    }
    /*entity realm = zox_get_child_by_id(
        world,
        game,
        zox_id(Realm));*/
    /*if (!zox_valid(realm)) {
        zox_loge("[game_state_terrain] Invalid [realm]");
        return;
    }*/
    lint realm_seed = zox_getv(realm, Seed);
    lint terrain_seed = realm_seed;
    // for now just do this
    if (zox_game_type == zox_game_mode_3D) {
        spawn_terrain(
            world,
            realm,
            terrain_seed);
    } else if (zox_game_type == zox_game_mode_2D) {
        spawn_grid2D(world);
    }
}
