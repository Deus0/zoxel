// a camera's effects, its pass' uniforms (fun/pass.c), set on the camera3 prefab (pre/_.c)
zoxc_float(CameraBlur);
zoxc_float(CameraVignette);
zoxc_float(CameraTint);     // under water: the view tinted deep blue, 0 to 1 (eased, src/terrains/voxel/sys/water_view.c)
zoxc_float(CameraLids);     // the eyes closing, 0 open to 1 shut: lids from the top and bottom (render_texture frag, xr eye_render.c)
// its settings, each its own tag (a setting's observer finds it by it, not its name)
zox_tag(SettingCameraOutlines);
zox_tag(SettingTonemap);

void zox_components_post_processing(ecs* world) {
    zoxd_float(CameraBlur);
    zoxd_float(CameraVignette);
    zoxd_float(CameraTint);
    zoxd_float(CameraLids);
    zoxd_tag(SettingCameraOutlines);
    zoxd_tag(SettingTonemap);
}
