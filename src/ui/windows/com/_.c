zox_tag(Window);
zox_tag(WindowSearchBar);       // a strip under a window header: its search (ins/search.c)
zox_tag(WindowSearchButton);
zox_tag(UndockButton);
zox_tag(WindowBody);
zox_tag(Header);
zox_tag(WindowFooter);     // a strip under a window's body, its buttons on it (spawn_window_footer)
zox_tag(CloseButton);
zox_tag(HeaderButton);     // a window's own header button, packed left of its search button (window_header_fit)
zox_tag(TabButton);        // a window's tab, its Target its tab's scrollview (ins/tabs.c)
zox_tag(WindowRaycastTarget);
zox_tag(WindowResizeHandle);
zoxc_byte(WindowLayer);
zox_tag(IgnoreCanvasStack);
zox_tag(WindowLayerDirty);
zoxc_entity(WindowTarget);
zoxc_byte(WindowsLayers);
zoxc_byte(WindowsCount);

void zox_components_windows(ecs* world) {
    zoxd_tag(Window);
    zoxd_tag(WindowSearchBar);
    zoxd_tag(WindowSearchButton);
    zoxd_tag(UndockButton);
    zoxd_tag(WindowBody);
    zoxd_tag(Header);
    zoxd_tag(WindowFooter);
    zoxd_tag(CloseButton);
    zoxd_tag(HeaderButton);
    zoxd_tag(TabButton);
    zoxd_tag(WindowRaycastTarget);
    zoxd_tag(WindowResizeHandle);
    zoxd_entity(WindowTarget);
    zoxd_byte(WindowLayer);
    zoxd_tag(WindowLayerDirty);
    zoxd_tag(IgnoreCanvasStack);
    zoxd_byte(WindowsLayers);
    zoxd_byte(WindowsCount);
}
