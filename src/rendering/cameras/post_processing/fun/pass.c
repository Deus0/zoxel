// a render texture's pass, set as it draws (rendering2's render_ui): its camera's effects, the tonemap, bloom and
//  outlines
//  - returns whether it bound the camera's depth and mask (unbound after the draw)
byte render_texture_post(ecs* world, entity render_texture, entity material) {
    const entity camera = zox_get_link(world, render_texture, CameraLink);
    const byte valid = zox_valid(camera);
    zox_gpu_float(zox_getv(material, MaterialBlur), valid && zox_has(camera, CameraBlur) ? zox_getv(camera, CameraBlur) : 0);
    zox_gpu_float(zox_getv(material, MaterialVignette), valid && zox_has(camera, CameraVignette) ? zox_getv(camera, CameraVignette) : 0);
    zox_gpu_float(zox_getv(material, MaterialTint), valid && zox_has(camera, CameraTint) ? zox_getv(camera, CameraTint) : 0);
    zox_gpu_float(zox_getv(material, MaterialLids), valid && zox_has(camera, CameraLids) ? zox_getv(camera, CameraLids) : 0);
    const guint material_id = zox_getv(material, MaterialGPULink);
    render_texture_tonemap(material_id);
    const gint bloom = zox_gpu_get_material_property(material_id, "bloom");
    if (bloom >= 0) {
        zox_gpu_float(bloom, zox_camera_bloom() ? zox_bloom_strength : 0);
    }
    return render_texture_outlines(world, render_texture, material_id);
}
