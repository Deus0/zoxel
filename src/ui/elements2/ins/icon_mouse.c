entity spawn_mouse_icon(ecs *world, entity prefab, entity canvas, int2 canvas_size, byte layer, float2 anchor, byte size, entity zevice) {
    int2 position = int2_zero; //  int2_half(canvas_size);
    entity e = spawn_uic(world, prefab, canvas, anchor, position, int2_single(size), int2_single(size), default_fill_color_icon, default_outline_color_icon);
    zox_add(e, MouseElement);
    zox_setv(e, Layer, layer);
    zox_setv(e, ZeviceLink, zevice);
    zox_setv(e, DataLink, 0);
    zox_setv(e, RenderDisabled, 1);
    return e;
}
