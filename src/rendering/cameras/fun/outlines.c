// outlines (doc/plans/outlines.md): a render texture's pass (render_texture_rgb.frag) reads its camera's depth and
//  outline mask, on texture units 1 and 2; off where the camera has none, outlines are off or unsupported
// a line's width in world units: a block texture's texel (a block's 0.5 over its 16), as the outlines baked into the
//  blocks' textures (textures/gen/noise.c, zox_outline_type_full), so models and terrain match
float zox_outline_width = 0.5f / 16.0f;

// returns whether it bound the camera's depth and mask (unbound after the draw)
byte render_texture_outlines(ecs* world, entity render_texture, guint material) {
    const gint data = zox_gpu_get_material_property(material, "outline_data");
    if (data < 0) {
        return 0;   // a shader without the pass (gles2)
    }
    const entity camera = zox_get_link(world, render_texture, CameraLink);
    const guint depth = zox_valid(camera) && zox_has(camera, RenderBufferLink) ? zox_getv(camera, RenderBufferLink) : 0;
    if (!zox_gpu_outlines_supported() || !depth || !zox_gpu_rbo_mask(depth)) {
        zox_gpu_float4(data, float4_zero);
        return 0;
    }
    // bound when off too: a sampler's image keeps its type (vulkan checks an integer mask's)
    zox_gpu_bind_texture_unit(1, depth);
    zox_gpu_bind_texture_unit(2, zox_gpu_rbo_mask(depth));
    zox_gpu_int(zox_gpu_get_material_property(material, "depth_texture"), 1);
    zox_gpu_int(zox_gpu_get_material_property(material, "mask_texture"), 2);
    if (!zox_camera_outlines) {
        zox_gpu_float4(data, float4_zero);
        return 1;
    }
    // pixels a world unit spans at a depth of 1: the texture's height over the view's
    const float height = zox_getv(render_texture, TextureSize).y;
    const float focal = height / (2 * tanf(zox_getv(camera, FieldOfView) * 0.5f * degreesToRadians));
    // near and far as the projection has them (its z column), so depth matches the mask's exactly
    const float4x4 projection = zox_getv(camera, ProjectionMatrix);
    const float plane_near = projection.w.z / (projection.z.z - 1);
    const float plane_far = projection.w.z / (projection.z.z + 1);
    zox_gpu_float4(data, (float4) { 1, plane_near, plane_far, zox_outline_width * focal });
    return 1;
}

// a render texture's tonemap (doc/plans/hdr.md): the setting, set on its pass' material each draw
void render_texture_tonemap(guint material) {
    const gint tonemap = zox_gpu_get_material_property(material, "tonemap");
    if (tonemap >= 0) {
        zox_gpu_float(tonemap, zox_tonemap);
    }
}
