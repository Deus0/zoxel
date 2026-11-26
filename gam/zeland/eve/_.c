// where the camera starts a game, over the origin, looking down at it
const float3 zeland_camera_position = { 0, 24, -32 };

// the game's state changed (its core Dirty): at boot, the start menu
void game_state_zeland(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GameState, state);
        if (state->value == zox_game_boot) {
            spawn_boot_menu(world);
        }
    }
}

// a player loading into a game: its camera flies free (right click toggles it), then it plays
void player_state_zeland(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        if (state->value != zox_player_state_loading) {
            continue;
        }
        const entity camera = zox_get_link(world, e, CameraLink);
        if (zox_valid(camera)) {
            const float4 rotation = camera_look_at(zeland_camera_position, float3_zero);
            zox_setv(camera, Position3D, zeland_camera_position);
            zox_setv(camera, Euler, quaternion_to_euler(rotation));
            zox_setv(camera, Rotation3D, rotation);
            zox_setv(camera, CameraState, zox_camera_state_free);
            zox_add(camera, CanRoam);
            zox_add(camera, Roaming);
        }
        zox_setv(e, PlayerStateTarget, zox_player_state_play_trigger);
    }
}

void zox_events_zeland(ecs* world) {
    zox_on_add(game_state_zeland, [in] games.GameState, [none] games.Game, [none] core.Dirty);
    zox_on_add(player_state_zeland, [in] players.PlayerState, [none] players.Player, [none] core.Dirty);
}
