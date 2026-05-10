entity spawn_icon_mouse_follow_canvas(ecs *world, entity prefab, entity canvas, int2 csize, byte layer, float2 anchor, int size, entity zevice) {
    int2 position = int2_half(csize);
    entity e = spawn_uic(world, prefab, canvas, anchor, position, int2_single(size), int2_single(size), default_fill_color_icon, default_outline_color_icon);
    zox_set(e, Layer2D, { layer });
    if (zevice) {
        zox_set(e, ZeviceLink, { zevice });
    }
    return e;
}
