// Canvases
zox_tag(Canvas);
zox_tag(BoundToCanvas);
zoxc_entity(CanvasLink);
zoxc_byte(Layer2D);
zoxc_byte(BonusLayer2); // for shifting things above others

// Positions
zoxc_int2(LayoutPosition);
zoxc_int2(CanvasPosition);
zoxc_state(LayoutPositionDirty);
zoxc_int4(LayoutConstraints);
zoxc_float2(Anchor);            // TODO: Rename: AnchorPosition

// Sizes
zoxc_int2(LayoutSize);
zoxc_state(LayoutSizeDirty);
zoxc_float2(AnchorSize);

// Windows - Top level in canvas
zoxc_entity(WindowToTop);

// Lists
zoxc_byte2(ListMargins);
zoxc_byte2(ListPadding);
zoxc_byte(ListStart);
zoxc_byte(ListVisible);
zoxc_byte(ListAlignment);
zoxc_state(ListDirty);
zoxc_state(ListPositionDirty);

// Grids
zoxc_byte2(GridSize);
zoxc_byte2(GridMargins);
zoxc_byte2(GridPadding);
zoxc_state(GridDirty);

void define_components_layouts2(ecs* world) {
    // Canvases
    zoxd_tag(Canvas);
    zoxd_tag(BoundToCanvas);
    zoxd_entity(CanvasLink);
    zoxd_byte(Layer2D);
    zoxd_byte(BonusLayer2);

    // local positions
    zoxd_int2(LayoutPosition);
    zoxd_state(LayoutPositionDirty);
    zoxd_int4(LayoutConstraints);
    zoxd_int2(CanvasPosition);
    zoxd_float2(Anchor);

    // sizes
    zoxd_int2(LayoutSize);
    zoxd_state(LayoutSizeDirty);
    zoxd_float2(AnchorSize);

    // Windows
    zoxd_entity(WindowToTop);

    // Grids
    zoxd_byte2(GridSize);
    zoxd_byte2(GridMargins);
    zoxd_byte2(GridPadding);
    zoxd_state(GridDirty);

    // Lists
    zoxd_byte(ListStart);
    zoxd_byte(ListVisible);
    zoxd_byte(ListAlignment);
    zoxd_byte2(ListMargins);
    zoxd_byte2(ListPadding);
    zoxd_state(ListDirty);
    zoxd_state(ListPositionDirty);

}
