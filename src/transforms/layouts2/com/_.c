zox_tag(Canvas);
zox_tag(BoundToCanvas);
zoxc_int2(LayoutPosition);
zoxc_int2(LayoutSize);
zoxc_int2(CanvasPosition);
zoxc_int4(LayoutConstraints);
zoxc_float2(Anchor);
zoxc_float2(AnchorSize);
zoxc_entity(CanvasLink);
zoxc_byte(Layer2D);
zoxc_entity(WindowToTop);
zoxc_byte2(ListMargins);
zoxc_byte2(ListPadding);
zoxc_byte2(GridMargins);
zoxc_byte2(GridPadding);
zoxc_byte(ListStart);
zoxc_byte(ListVisible);
zoxc_byte(ListUIMax);   // TODO: remove just use children length

zoxc_state(LayoutPositionDirty);
zoxc_state(LayoutSizeDirty);
zoxc_state(ListDirty);
zoxc_state(GridDirty);

void define_components_layouts2(ecs* world) {
    zoxd_tag(Canvas);
    zoxd_tag(BoundToCanvas);
    zoxd_int2(LayoutPosition);
    zoxd_int2(LayoutSize);
    zoxd_int2(CanvasPosition);
    zoxd_int4(LayoutConstraints);
    zoxd_float2(Anchor);
    zoxd_float2(AnchorSize);
    zoxd_entity(CanvasLink);
    zoxd_byte(Layer2D);
    zoxd_entity(WindowToTop);
    zoxd_byte2(ListMargins);
    zoxd_byte2(ListPadding);
    zoxd_byte2(GridMargins);
    zoxd_byte2(GridPadding);
    zoxd_byte(ListStart);
    zoxd_byte(ListVisible);
    zoxd_byte(ListUIMax);
    zoxd_state(LayoutPositionDirty);
    zoxd_state(LayoutSizeDirty);
    zoxd_state(ListDirty);
    zoxd_state(GridDirty);
}