zox_tag(Window);
zox_tag(WindowBody);
zox_tag(Header);
zox_tag(WindowFooter);     // a strip under a window's body, its buttons on it (spawn_window_footer)
zox_tag(CloseButton);
zox_tag(WindowRaycastTarget);
zoxc_byte(WindowLayer);
zox_tag(IgnoreCanvasStack);
zox_tag(WindowLayerDirty);
zoxc_entity(WindowTarget);
zoxc_byte(WindowsLayers);
zoxc_byte(WindowsCount);

void zox_components_windows(ecs* world) {
    zoxd_tag(Window);
    zoxd_tag(WindowBody);
    zoxd_tag(Header);
    zoxd_tag(WindowFooter);
    zoxd_tag(CloseButton);
    zoxd_tag(WindowRaycastTarget);
    zoxd_entity(WindowTarget);
    zoxd_byte(WindowLayer);
    zoxd_tag(WindowLayerDirty);
    zoxd_tag(IgnoreCanvasStack);
    zoxd_byte(WindowsLayers);
    zoxd_byte(WindowsCount);
}
