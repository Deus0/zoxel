zox_tag(Window);
zox_tag(WindowBody);
zox_tag(Header);
zox_tag(CloseButton);
zox_tag(WindowRaycastTarget);
zoxc_byte(WindowLayer);
zox_tag(IgnoreCanvasStack);
zox_tag(WindowLayerDirty);
zoxc_entity(WindowRaycasted);
zoxc_entity(WindowTarget);
zoxc_byte(WindowsLayers);
zoxc_byte(WindowsCount);

void define_components_windows(ecs* world) {
    zoxd_tag(Window);
    zoxd_tag(WindowBody);
    zoxd_tag(Header);
    zoxd_tag(CloseButton);
    zoxd_tag(WindowRaycastTarget);
    zoxd_entity(WindowRaycasted);
    zoxd_entity(WindowTarget);
    zoxd_byte(WindowLayer);
    zoxd_tag(WindowLayerDirty);
    zoxd_tag(IgnoreCanvasStack);
    zoxd_byte(WindowsLayers);
    zoxd_byte(WindowsCount);
}
