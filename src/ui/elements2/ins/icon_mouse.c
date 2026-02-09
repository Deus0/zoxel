entity spawn_icon_mouse_follow(ecs *world, SpawnIcon *data) {

    entity e = spawn_icon(world, data).x;

    icon_mouse_follow = e;

    return e;
}

entity spawn_icon_mouse_follow_canvas(
    ecs *world,
    entity prefab,
    entity canvas,
    int2 canvas_size,
    byte layer,
    float2 anchor,
    int icon_size,
    entity zevice
) {
    SpawnIcon spawnIcon = {
        .canvas = { .e = canvas },
        .parent = {
            .e = canvas,
            .position = int2_half(canvas_size),
            .size = canvas_size
        },
        .element = {
            .prefab = prefab,
            .size = int2_single(icon_size),
            .anchor = anchor,
            .layer = layer,
            .render_disabled = 1,
        },
        .texture = {
            .fill_color = default_fill_color_icon,
            .outline_color = default_outline_color_icon
        }
    };

    entity e = spawn_icon(world, &spawnIcon).x;

    if (zevice) {
        zox_set(e, ZeviceLink, { zevice });
    }

    return e;
}
