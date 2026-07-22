entity spawn_render_texture_canvas(ecs *world, entity prefab, entity canvas, int2 layout_size, int2 texture_size, entity camera) {
    byte layer = 0;
    entity parent = canvas;
    int2 position = int2_zero;
    float2 anchor = float2_half;
    zox_instance(prefab);
    zox_name("render_texture");
    initialize_element(world, e, parent, canvas, position, layout_size, texture_size, anchor, layer);
    zox_set(camera, RenderTextureLink, { e });
    zox_set(e, CameraLink, { camera });
    zox_set(e, MaterialLink, { material_render_texture });
    // zox_log("render texture s %ix%i ts %ix%i", layout_size.x, layout_size.y, texture_size.x, texture_size.y);
    return e;
}
