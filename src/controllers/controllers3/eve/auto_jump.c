// the "Auto Jump" setting, on by default when a touchscreen is found
void auto_jump_settings(ecs* world, entity app) {
    auto_jump = sdl_get_touch_device_count() > 0;
    setting_tags(world, spawn_setting_byte(world, app, "Auto Jump", auto_jump), zox_id(SettingTabGame), zox_id(SettingAutoJump));
}

void auto_jump_settings_on_dirty(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZoxName, name);
        if (zox_has(e, SettingByte) && zox_has(e, SettingAutoJump)) {
            auto_jump = zox_getv(e, SettingByte);
        }
    }
}
