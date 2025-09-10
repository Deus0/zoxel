zox_tag(App);
zoxc_state(WindowSizeDirty);
zoxc_byte(WindowFullscreen);
zoxc_byte(WindowMaximized);
zoxc_byte(WindowMonitor);
zoxc_int2(WindowSize);
zoxc_int2(WindowPosition);
zoxc_int2(WindowSizeRestore);
zoxc_int2(WindowPositionRestore);
zoxc_entity(AppLink);

void define_components_apps(ecs_world_t *world) {
    zoxd_tag(App);
    zoxd_state(WindowSizeDirty);
    zoxd_byte(WindowFullscreen);
    zoxd_byte(WindowMaximized);
    zoxd_byte(WindowMonitor);
    zoxd_int2(WindowSize);
    zoxd_int2(WindowPosition);
    zoxd_int2(WindowSizeRestore);
    zoxd_int2(WindowPositionRestore);
    zoxd_entity(AppLink);
#ifdef zox_vulkan
    zoxd(VulkanSurface);
#endif
}