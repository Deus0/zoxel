// Lists
zoxc_byte2(ListMargins);
zoxc_byte2(ListPadding);
zoxc_byte(ListStart);
zoxc_byte(ListVisible);
zoxc_byte(ListAlignment);
zoxc_byte(ListIndent);
// Grids
zoxc_byte2(GridSize);
zoxc_byte2(GridMargins);
zoxc_byte2(GridPadding);
// Events
zoxc_dirty(Grid);
zoxc_state(ListDirty);
zoxc_state(ListPositionDirty);

void zox_components_containers(ecs* world) {
    // Lists
    zoxd_byte(ListStart);
    zoxd_byte(ListVisible);
    zoxd_byte(ListAlignment);
    zoxd_byte2(ListMargins);
    zoxd_byte2(ListPadding);
    zoxd_byte(ListIndent);
    // Grids
    zoxd_byte2(GridSize);
    zoxd_byte2(GridMargins);
    zoxd_byte2(GridPadding);
    // Events
    zoxd_state(ListDirty);
    zoxd_state(ListPositionDirty);
    zoxd_dirty(Grid);
}
