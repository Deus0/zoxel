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
    const entity bloom = spawn_setting_byte(world, app, "Bloom", zox_bloom);
    setting_tags(world, bloom, zox_id(SettingTabVideo), zox_id(SettingBloom));
    if (!zox_gpu_hdr_supported() || !zox_gpu_bloom_supported() || zox_xr_active) {
        zox_add(bloom, SettingUnsupported);
    }
    // the screen itself in hdr (fun/output.c): vulkan with an hdr screen; off on others shows the same
    const byte hdr_supported = zox_gpu_hdr_output_supported() && !zox_xr_active;
    zox_logv("[hdr] output supported [%i]", hdr_supported);
    const entity hdr = spawn_setting_byte(world, app, "HDR", zox_hdr_output_asked != zox_hdr_output_off);
    setting_tags(world, hdr, zox_id(SettingTabVideo), zox_id(SettingHdr));
    if (!hdr_supported) {
        zox_add(hdr, SettingUnsupported);
        // greyed means off: not asked for, whatever --hdr or the settings file said
        if (zox_hdr_output_asked != zox_hdr_output_canvas) {
            zox_gpu_set_hdr_output(zox_hdr_output_off);
        }
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
        } else if (zox_has(e, SettingBloom)) {
            zox_set_bloom(world, value);
        } else if (zox_has(e, SettingHdr) && zox_hdr_output_asked != zox_hdr_output_canvas) {
            // unsupported (greyed): kept off, a saved or passed on value ignored
            const byte supported = !zox_has(e, SettingUnsupported);
            zox_gpu_set_hdr_output(value && supported ? zox_hdr_output_hdr10 : zox_hdr_output_off);
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
