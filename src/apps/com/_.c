zox_tag(App);
zox_tag(AppLink);
zoxc_byte(WindowFullscreen);
zoxc_byte(WindowMaximized);
zoxc_byte(WindowMonitor);
zoxc_int2(WindowSize);
zoxc_int2(WindowPosition);
zoxc_int2(WindowSizeRestore);
zoxc_int2(WindowPositionRestore);
zox_tag(WindowSizeDirty);

// its settings, each its own tag (a setting's observer finds it by it, not its name)
zox_tag(SettingFullscreen);
zox_tag(SettingVsync);

void zox_components_apps(ecs *world) {
    zoxd_tag(SettingFullscreen);
    zoxd_tag(SettingVsync);

    zoxd_tag(App);
    zoxd_nf_link(AppLink);
    zoxd_byte(WindowFullscreen);
    zoxd_byte(WindowMaximized);
    zoxd_byte(WindowMonitor);
    zoxd_int2(WindowSize);
    zoxd_int2(WindowPosition);
    zoxd_int2(WindowSizeRestore);
    zoxd_int2(WindowPositionRestore);
    zoxd_nf_tag(WindowSizeDirty);
}
