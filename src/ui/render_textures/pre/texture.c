entity spawn_prefab_render_texture(ecs *world, entity prefab) {
    zox_prefab_child(prefab);    // from element_basic
    zox_add_tag(e, RenderTexture);
    zox_prefab_set(e, CameraLink, { 0 });
    zox_prefab_set(e, MaterialLink, { 0 });
    // for now we must do this otherwise we get edge pixels
    float scale1 = 1; // 1.001f;
    // no 2D matrix system yet
    zox_prefab_set(e, Scale1D, { scale1 });
    zox_prefab_set(e, TransformMatrix, {
        float4x4_transform_scale(float3_zero, quaternion_identity, scale1)
    });
    zox_prefab_set(e, Brightness, { 1 });
    return e;
}
