entity spawn_prefab_render_texture(
    ecs *world,
    entity prefab)
{
    zox_prefab_child(prefab);    // from element_basic
    zox_add(e, RenderTexture);
    zox_add(e, RenderTextureDirty);
    // zox_setv(e, MaterialLink, 0);
    // for now we must do this otherwise we get edge pixels
    zox_setv(e, TransformMatrix, float4x4_identity);
    return e;
}
