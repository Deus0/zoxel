// zoxc_byte(WindowType);
zox_tag(Window);
zox_tag(WindowRaycastTarget);
zox_tag(IgnoreWindowLayering);
zoxc_byte(WindowLayer);
zoxc_byte(SetWindowLayer);
zoxc_entity(WindowRaycasted);
zoxc_entity(WindowTarget);
zoxc_byte(HeaderHeight);
zoxc_byte(WindowsLayers);
zoxc_byte(WindowsCount);
// Move to Windows
zox_tag(Header);
zox_tag(CloseButton);

void define_components_windows(ecs* world) {
    zoxd_tag(Window);
    // zoxd_byte(WindowType);
    // Windows
    zoxd_tag(WindowRaycastTarget);
    zoxd_tag(IgnoreWindowLayering);
    zoxd_entity(WindowRaycasted);
    zoxd_entity(WindowTarget);
    zoxd_byte(SetWindowLayer);
    zoxd_byte(WindowLayer);
    zoxd_byte(HeaderHeight);
    zoxd_byte(WindowsLayers);
    zoxd_byte(WindowsCount);
    // Move to Windows
    zoxd_tag(Header);
    zoxd_tag(CloseButton);
}
