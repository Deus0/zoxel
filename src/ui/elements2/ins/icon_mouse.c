/*entity spawn_icon_mouse_follow(ecs *world, SpawnIcon data) {
    entity e = spawn_icon(world, data).x;
    icon_mouse_follow = e;
    return e;
}*/

entity spawn_icon_mouse_follow_canvas(ecs *world, entity p, entity canvas, int2 csize, byte layer, float2 anchor, int size, entity zevice) {
    int2 position = int2_half(csize);
    entity e = spawn_element2(world, p, canvas, canvas, position, int2_single(size), int2_single(size), anchor, layer, default_fill_color_icon, default_outline_color_icon);
    if (zevice) {
        zox_set(e, ZeviceLink, { zevice });
    }
    return e;
}
