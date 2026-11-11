// Canvases
zox_tag(Layout);
zox_tag(Canvas);
zox_tag(Canvas3);
zox_tag(BoundToCanvas);
zox_tag(BoundToParent);
zox_tag(CanvasLink);
zoxc_byte(Layer);
zoxc_byte(BonusLayer); // for shifting things above others
// Positions
zoxc_int2(LayoutPosition);
zoxc_int2(CanvasPosition);
zoxc_int4(LayoutConstraints);
zoxc_float2(Anchor);            // TODO: Rename: AnchorPosition
// Sizes
zoxc_int2(LayoutSize);
zoxc_float2(AnchorSize);
// Windows - Top level in canvas
zoxc_entity(WindowToTop);
// Lines
zoxc_float2(LineAnchor);
typedef struct {
    int2 start;
    int2 end;
} LayoutLinePoints;
zoxc_custom(LayoutLinePoints);
// events
zoxc_dirty(LayoutPosition);
zoxc_dirty(LayoutSize);

void zox_components_layouts2(ecs* world) {
    // Canvases
    zoxd_tag(Layout);
    zoxd_tag(Canvas);
    zoxd_tag(Canvas3);
    zoxd_tag(BoundToCanvas);
    zoxd_tag(BoundToParent);
    zoxd_nf_link(CanvasLink);
    zoxd_byte(Layer);
    zoxd_byte(BonusLayer);
    // local positions
    zoxd_int2(LayoutPosition);
    zoxd_int4(LayoutConstraints);
    zoxd_int2(CanvasPosition);
    zoxd_float2(Anchor);
    // sizes
    zoxd_int2(LayoutSize);
    zoxd_float2(AnchorSize);
    // Windows
    zoxd_entity(WindowToTop);
    // hmm
    zoxd_float2(LineAnchor);
    zoxd(LayoutLinePoints);
    // events
    zoxd_dirty(LayoutPosition);
    zoxd_dirty(LayoutSize);
}
