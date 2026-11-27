// the camera drawing now, added before every shader as it loads (src/rendering/shd/fun/view.c, doc/howto/shaders.md)
//  - src/rendering/dat/camera_renderer.c's RenderView, uploaded once a camera
//  - mediump: a block's members must have one precision in both stages (gles), the fragment's (shd/fun/precision.c);
//    highp ones mixed into a mediump fragment's maths drew nothing on vulkan (lavapipe, tst body_views)
layout(std140) uniform RenderView {
    mediump vec4 fog_data;    // the fog's colour (xyz) and density (w)
};

// how much fog covers a point, fog_level its depth squared (lines and particles fade their own way)
float zox_fog_blend(float fog_level) {
    return min(1.0, 1.1 - exp2(-fog_data.w * fog_level));
}
