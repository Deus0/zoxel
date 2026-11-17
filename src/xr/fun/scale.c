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

void xr_settings_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LoadSettings);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LoadSettings, load);
        if (load->value != zox_load_settings_spawn) {
            continue;
        }
        spawn_setting_float(world, e, "XR Height", xr_player_height, xr_player_height_range);
    }
    zox_sys_on_end();
} zoxd_system(xr_settings_system);

void xr_settings_dirty_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SettingDirty);
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SettingDirty, state);
        zox_sys_i(ZoxName, name);
        if (state->value != zox_dirty_active || !zox_has(e, SettingFloat)) {
            continue;
        }
        if (!strcmp(name->value, "XR Height")) {
            xr_player_height = zox_getv(e, SettingFloat);
        }
    }
    zox_sys_on_end();
} zoxd_system(xr_settings_dirty_system);

void zox_systems_xr_scale(ecs* world) {
    // main thread: it spawns the setting
    zox_system_1(
        xr_settings_system,
        zoxp_spawn,
        [in] settings.LoadSettings,
    );
    zox_system(
        xr_settings_dirty_system,
        zoxp_update,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
