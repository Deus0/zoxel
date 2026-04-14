// Should we add a dirty flag??
zox_sys2(CameraBlurSystem) {
    entity mat = material_render_texture;
    zox_sys_world();
    // uint gpu_blur = zox_gett(mat, MaterialAttributesRenderTexture)->blur_strength;
    zox_sys_begin();
    zox_sys_in(CameraBlur);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(CameraBlur, blur);

        // TODO: Material Link on Camera, RenderTextureLink, then MaterialLink

        zox_set(mat, CameraBlur, { blur->value });

        // zox_gpu_float(gpu_blur, blur->value);
        // zox_log("Set Material Blur [%s] %f", zox_get_name(mat), blur->value);
    }
} zox_sys_end(CameraBlurSystem);
