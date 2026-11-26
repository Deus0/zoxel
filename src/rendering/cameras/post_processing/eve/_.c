// its settings, spawned on the app (InitializeEvent), applied when one changes (an observer on Dirty)
void app_initialize_post_processing(ecs* world, entity app) {
    const entity outlines = spawn_setting_byte(world, app, "Camera Outlines", zox_camera_outlines);
    setting_tags(world, outlines, zox_id(SettingTabVideo), zox_id(SettingCameraOutlines));
    // xr's eyes draw straight to their swapchains, no pass yet (todo.md)
    if (!zox_gpu_outlines_supported() || zox_xr_active) {
        zox_add(outlines, SettingUnsupported);
    }
    // doc/plans/hdr.md: 0 clamp, 1 soft, 2 filmic, for a float target's colours past 1
    const entity tonemap = spawn_setting_byte_slider(world, app, "Tonemap", zox_tonemap, (byte2) { 0, 2 });
    setting_tags(world, tonemap, zox_id(SettingTabVideo), zox_id(SettingTonemap));
    if (!zox_gpu_hdr_supported() || zox_xr_active) {
        zox_add(tonemap, SettingUnsupported);
    }
}

void post_processing_settings_on_dirty(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        if (!zox_has(e, SettingByte)) {
            continue;
        }
        const byte value = zox_getv(e, SettingByte);
        if (zox_has(e, SettingCameraOutlines)) {
            zox_camera_outlines = value;
        } else if (zox_has(e, SettingTonemap)) {
            zox_set_tonemap(world, value);
        }
    }
}

void zox_events_post_processing(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, app_initialize_post_processing);
    zox_on_add(
        post_processing_settings_on_dirty,
        [none] settings.Setting,
        [none] core.Dirty,
    );
}
