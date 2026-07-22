// Canvases
zox_tag(Layout);
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

void define_components_layouts2(ecs* world) {
    // Canvases
    zoxd_tag(Layout);
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
}
