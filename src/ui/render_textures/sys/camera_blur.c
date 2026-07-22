// Should we add a dirty flag??
zox_sys2(CameraBlurSystem) {
    entity material = material_render_texture;
    zox_sys_world();
    if (!zox_valid(material) || !zox_has(material, MaterialBlur)) {
        zox_log_error("Invalid [material_render_texture]");
        return;
    }
    guint blur_id = zox_getv(material, MaterialBlur);
    // zox_geter(material, MaterialAttributesRenderTexture, attributes);
    // uint gpu_blur = zox_gett(mat, MaterialAttributesRenderTexture)->blur_strength;
    zox_sys_begin();
    zox_sys_in(CameraBlur);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(CameraBlur, blur);
        // TODO: Material Link on Camera, RenderTextureLink, then MaterialLink
        zox_set(material, CameraBlur, { blur->value });
        //if (zox_has(mat, CameraBlur)) {
        // zox_geter_value(mat,  CameraBlur, float, blur);
        zox_gpu_float(blur_id, blur->value);
        // zox_log("Set Camera Blur [%s] %f", zox_get_name(camera->value), blur);
        //}
        // zox_gpu_float(gpu_blur, blur->value);
        // zox_log("Set Material Blur [%s] %f", zox_get_name(mat), blur->value);
    }
} zox_sys_end(CameraBlurSystem);
