zox_tag(BoundToCanvas);
zoxc_int2(PixelPosition);
zoxc_byte(LayoutPositionDirty);
zoxc_int2(PixelSize);
zoxc_byte(LayoutSizeDirty);
zoxc_int2(CanvasPosition);
zoxc_int4(DraggableLimits);
zoxc_float2(Anchor);
zoxc_float2(AnchorSize);
zoxc_entity(CanvasLink);
zoxc_byte(Layer2D);
zoxc_entity(WindowToTop);

void define_components_layouts2(ecs* world) {
    zox_define_tag(BoundToCanvas);
    zox_define_component_int2(PixelPosition);
    zoxd_byte(LayoutPositionDirty);
    zox_define_component_int2(PixelSize);
    zoxd_byte(LayoutSizeDirty);
    zox_define_component_int2(CanvasPosition);
    zox_define_component_int4(DraggableLimits);
    zox_define_component_float2(Anchor);
    zox_define_component_float2(AnchorSize);
    zox_define_component_entity(CanvasLink);
    zoxd_byte(Layer2D);
    zox_define_component_entity(WindowToTop);
}