zox_tag(Line3D);
zoxc(LineData3D, float6);
zox_tag(CubeLines);
zoxc_byte(DebugCubeLines);
zox_tag(DebugCubeCorner);
zoxc_float(DebugCubeShrink);
zox_tag(QuadLines);
zoxc_float(QuadLineSize);
zoxc_float(CubeLineExtrude);
zoxc_float(CubeLineShrink);
zoxc_byte(CubeLineSides);

void define_components_lines3(ecs* world) {
    zoxd_tag(Line3D);
    zoxd_tag(CubeLines);
    zoxd_tag(DebugCubeCorner);
    zoxd(DebugCubeLines);
    zoxd(LineData3D);
    zoxd_float(DebugCubeShrink);
    zoxd_tag(QuadLines);
    zoxd_float(QuadLineSize);
    zoxd_float(CubeLineExtrude);
    zoxd_float(CubeLineShrink);
    zoxd_byte(CubeLineSides);
}
