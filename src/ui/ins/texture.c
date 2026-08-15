// NOTE: Spawn an element with a texture
entity spawn_element_texture(ecs* world, entity prefab, entity parent, int2 position, int2 size, float2 anchor, byte layer, entity texture) {
    int2 texture_size = zox_getv(texture, TextureSize);
    zox_geter(texture, TextureData, source_data);
    entity e = spawn_ui(world, prefab, parent, anchor, position, size, texture_size);
    zox_name("element_texture");
    zox_set_ptr(e, TextureData, *source_data);
    zox_setv(e, TextureDirty, zox_upload_texture);
    zox_setv(e, Layer, layer);
    return e;
}
