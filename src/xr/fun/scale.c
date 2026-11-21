// XR scale: the player's real size onto their character's, so the body is theirs (its arms reach their hands)
//  - the character is about 1m tall (a humanoid fits 2 blocks, tst body_views), the player isn't: everything
//    tracked (the eyes, their distance apart, walking in the room, the hands) is scaled by the character's height
//    over the player's real height ("XR Height", settings), rotations aren't
//  - the world looks the size it does to the character (bigger), the hands land in the character's reach
//  - in menus (the roam flies, no body) the scale is 1
float xr_player_height = 1.7f;      // metres, setting: "XR Height"
#define xr_player_height_range (float2) { 1.2f, 2.2f }
float xr_view_scale = 1;            // the eyes' tracked positions scaled by it (fun/eye_render.c), this frame's

// the character's height over the player's, 1 without a character
static inline float xr_body_scale(ecs* world, entity character) {
    if (!zox_valid(character) || !zox_has(character, Bounds3D) || xr_player_height <= 0) {
        return 1;
    }
    const float height = zox_getv(character, Bounds3D).y * 2;
    return height > 0 ? height / xr_player_height : 1;
}

// its settings, spawned on the app (InitializeEvent)
void xr_settings(ecs* world, entity e) {
    spawn_setting_float(world, e, "XR Height", xr_player_height, xr_player_height_range);
}

// a setting changed (its Dirty, added again each frame it changes: a slider dragged)
void xr_settings_on_dirty(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZoxName, name);
        if (!zox_has(e, SettingFloat)) {
            continue;
        }
        if (!strcmp(name->value, "XR Height")) {
            xr_player_height = zox_getv(e, SettingFloat);
        }
    }
}

// spawned on the app (InitializeEvent), applied when one changes (an observer on Dirty)
void xr_settings_events(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, xr_settings);
    zox_on_add(
        xr_settings_on_dirty,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Dirty,
    );
}

void zox_systems_xr_scale(ecs* world) {
    // its settings: spawned on the app, applied when one changes (needs prefab_app)
    add_hook_spawn_prefabs(xr_settings_events);
}
