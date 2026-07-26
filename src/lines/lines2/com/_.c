zox_tag(Line2D);
zox_tag(ElementLine2D);
zoxc(LineData2D, float4);
// zoxc(LinePosition2, int4);
// zoxc_state(LineLocalPositionDirty);
// only these needed now
// zoxc(LineLocalPosition2, int4);
// zoxc(LineAnchor, float4);

void define_components_lines2(ecs* world) {
    zoxd_tag(Line2D);
    zoxd_tag(ElementLine2D);
    zoxd(LineData2D);
    // zoxd(LinePosition2);
    // zoxd(LineLocalPosition2);
    // zoxd_state(LineLocalPositionDirty);
}
