entity spawn_close_button(
    ecs *world,
    entity parent,
    entity canvas,
    byte size,
    byte padding,
    byte layer,
    ClickEvent on_click)
{
    byte font_thickness = close_button_font_thickness * ui_scale;
    byte fonto_thickness = close_button_fonto_thickness * ui_scale;
    float2 anchor = (float2) { 1, 0.5f };
    byte alignment = zox_alignment_right;
    SpawnButton button_data = {
        .canvas = { .e = canvas },
        .parent = { .e = parent },
        .element = {
            .prefab = prefab_close_button,
            .layer = layer,
            .anchor = anchor
        },
        .button = {
            .prefab_text = prefab_text,
            .fill = close_button_fill,
            .outline = close_button_outline,
        },
        .zext = {
            .alignment = alignment,
            .text = "X",
            .font_size = size,
            .font_resolution = size,
            .margins = byte2_single(padding),
            .font_thickness = font_thickness,
            .font_outline_thickness = fonto_thickness,
            .font_fill_color = close_button_font_fill,
            .font_outline_color = close_button_font_outline
        },
    };
    entity e = spawn_button_old(
        world,
        button_data.canvas,
        button_data.parent,
        button_data.element,
        button_data.zext,
        button_data.button);
    zox_set_unique_name(e, "close_button");
    zox_setv(e, MeshAlignment, alignment);
    if (on_click.value) {
        zox_setv(e, ClickEvent, on_click.value);
    } else {
        zox_logw("Close button has no event");
    }
    return e;
}
