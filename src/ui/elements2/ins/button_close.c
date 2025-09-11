entity spawn_close_button(
    ecs *world,
    const entity parent,
    const entity canvas,
    const int2 position,
    const int font_size,
    byte2 margins,
    const byte layer,
    ClickEvent on_click
) {
    const float2 anchor = (float2) { 1, 0.5f };
    SpawnButton button_data = {
        .canvas = { .e = canvas },
        .parent = { .e = parent },
        .element = {
            .prefab = prefab_close_button,
            .position = position,
            .layer = layer,
            .anchor = anchor
        },
        .zext = {
            .text = "X",
            .font_size = font_size,
            .font_thickness = 4,
            .margins = margins,
            .font_fill_color = close_button_font_fill,
            .font_outline_color = close_button_font_outline
        },
        .button = {
            .prefab_zext = prefab_zext,
            .fill = close_button_fill,
            .outline = close_button_outline,
        },
    };
    const entity e = spawn_button(
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
