zox_tag(Docked);
zox_tag(DockResizeHandle);   // a docked window's inner edge handle (ins/docks.c), not its window's own resize handles
zox_tag(DockPreview);
zox_tag(DockViewport);
zox_tag(DockDragLink);

static void zox_components_docks(ecs* world) {
    zoxd_tag(Docked);
    zoxd_tag(DockResizeHandle);
    zoxd_tag(DockPreview);
    zoxd_tag(DockViewport);
    zoxd_nf_link(DockDragLink);
}
