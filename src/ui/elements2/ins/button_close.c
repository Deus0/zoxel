entity spawn_close_button(ecs *world, entity parent, entity canvas, int2 position, int font_size, byte2 margins, byte layer, ClickEvent on_click) {

    float2 anchor = (float2) { 1, 0.5f };

    SpawnButton button_data = {
        .canvas = { .e = canvas },
        .parent = { .e = parent },
        .element = {
            .prefab = prefab_close_button,
            .position = position,
            .layer = layer,
            .anchor = anchor
        },
        .button = {
            .prefab_zext = prefab_zext,
            .fill = close_button_fill,
            .outline = close_button_outline,
        },
        .zext = {
            .text = "X",
            .font_size = font_size,
            .font_resolution = close_button_font_resolution, // font_size,
            .font_thickness = close_button_font_thickness,
            .font_outline_thickness = close_button_fonto_thickness,
            .margins = margins,
            .font_fill_color = close_button_font_fill,
            .font_outline_color = close_button_font_outline
        },
    };

    entity e = spawn_button(
        world,
        button_data.canvas,
        button_data.parent,
        button_data.element,
        button_data.zext,
        button_data.button
    );

    if (on_click.value) {
        zox_set(e, ClickEvent, { on_click.value });
    }

    return e;
}