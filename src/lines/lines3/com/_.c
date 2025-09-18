zox_tag(Line3D);
zoxc(LineData3D, float6);
zox_tag(CubeLines);
zoxc_byte(DebugCubeLines);
zoxc_float(CubeLinesThickness);
zox_tag(DebugCubeCorner);
zoxc_float(DebugCubeShrink);

void define_components_lines3(ecs* world) {
    zoxd_tag(Line3D);
    zoxd_tag(CubeLines);
    zoxd_tag(DebugCubeCorner);
    zoxd(DebugCubeLines);
    zoxd(LineData3D);
    zoxd_float(CubeLinesThickness);
    zoxd_float(DebugCubeShrink);
}