entity spawn_mouse_icon(ecs *world, entity prefab, entity canvas, int2 canvas_size, byte layer, float2 anchor, int size, entity zevice) {
    int2 position = int2_half(canvas_size);
    entity e = spawn_uic(world, prefab, canvas, anchor, position, int2_single(size), int2_single(size), default_fill_color_icon, default_outline_color_icon);
    zox_set(e, Layer2D, { layer });
    zox_add_tag(e, MouseElement);
    zox_set(e, RenderDisabled, { 1 });
    zox_set(e, ZeviceLink, { zevice });
    zox_set(e, DataLink, { 0 });
    return e;
}
