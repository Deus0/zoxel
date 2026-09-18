// Lists
zoxc_byte2(ListMargins);
zoxc_byte2(ListPadding);
zoxc_byte(ListStart);
zoxc_byte(ListVisible);
zoxc_byte(ListAlignment);
zoxc_byte(ListIndent);
zoxc_state(ListDirty);
zoxc_state(ListPositionDirty);

// Grids
zoxc_byte2(GridSize);
zoxc_byte2(GridMargins);
zoxc_byte2(GridPadding);
zoxc_state(GridDirty);

void zox_components_containers(ecs* world) {
    // Lists
    zoxd_byte(ListStart);
    zoxd_byte(ListVisible);
    zoxd_byte(ListAlignment);
    zoxd_byte2(ListMargins);
    zoxd_byte2(ListPadding);
    zoxd_byte(ListIndent);
    zoxd_state(ListDirty);
    zoxd_state(ListPositionDirty);
    // Grids
    zoxd_byte2(GridSize);
    zoxd_byte2(GridMargins);
    zoxd_byte2(GridPadding);
    zoxd_state(GridDirty);
}
