entity spawn_render_texture(ecs *world, entity prefab, entity parent, float2 anchor, int2 position, int2 size, int2 texture_size, byte layer, entity camera) {
    entity e = spawn_ui(world, prefab, parent, anchor, position, size, texture_size);
    zox_name("render_texture");
    zox_setv(e, Layer, layer);
    zox_setv(e, MaterialLink, material_render_texture);
    zox_setv(e, CameraLink, camera);
    zox_setv(camera, RenderTextureLink, e);
    return e;
}
