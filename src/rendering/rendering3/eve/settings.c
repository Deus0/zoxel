void app_initialize_rendering3(ecs* world, entity app) {
    setting_tags(world, spawn_setting_byte(world, app, "Grass Animation", zox_grass_animation), zox_id(SettingTabVideo), zox_id(SettingGrassAnimation));
    const entity outlines = spawn_setting_byte(world, app, "Camera Outlines", zox_camera_outlines);
    setting_tags(world, outlines, zox_id(SettingTabVideo), zox_id(SettingCameraOutlines));
    // xr's eyes draw straight to their swapchains, no pass yet (todo.md)
    if (!zox_gpu_outlines_supported() || zox_xr_active) {
        zox_add(outlines, SettingUnsupported);
    }
}

void rendering3_settings_observer_callback(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZoxName, name);
        if (!zox_has(e, SettingByte)) {
            continue;
        }
        byte value = zox_getv(e, SettingByte);
        if (dbg_log) {
            zox_log("Setting [rendering3] [byte] [%s] [%i]",
                name->value,
                value);
        }
        if (zox_has(e, SettingGrassAnimation)) {
            zox_grass_animation = value;
        } else if (zox_has(e, SettingCameraOutlines)) {
            zox_camera_outlines = value;
        }
    }
}
