zox_tag(App);
zoxc_state(WindowSizeDirty);
zoxc_byte(WindowFullscreen);
zoxc_byte(WindowMaximized);
zoxc_byte(WindowMonitor);
zoxc_int2(WindowSize);
zoxc_int2(WindowPosition);
zoxc_int2(WindowSizeRestore);
zoxc_int2(WindowPositionRestore);

void define_components_apps(ecs *world) {
    zoxd_nf_tag(App);
    zoxd_state(WindowSizeDirty);
    zoxd_byte(WindowFullscreen);
    zoxd_byte(WindowMaximized);
    zoxd_byte(WindowMonitor);
    zoxd_int2(WindowSize);
    zoxd_int2(WindowPosition);
    zoxd_int2(WindowSizeRestore);
    zoxd_int2(WindowPositionRestore);
#ifdef zox_vulkan
    zoxd(VulkanSurface);
#endif
}