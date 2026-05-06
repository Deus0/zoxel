// TODO: Spawn basic Window + Speech Text + Confirm Button

entity spawn_dialogue_ui(ecs* world, entity p, entity player) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);

    byte font_size = 16;

    SpawnWindow2 window_data = {
        .header_text = "Dialogue",
        .header_font_size = 24,
        .header_padding = (byte2) { 16, 16 },
    };
    LayoutParentData canvas_data = (LayoutParentData) {
        .e = canvas,
        .size = canvas_size
    };
    ElementSpawnData element_data = {
        .prefab = p,
        .size = (int2) { 720, 200 },
        .anchor = (float2) { 0.5f, 0.72f },
    };
    entity e = spawn_window2(world, canvas_data, canvas_data, element_data, window_data, (ClickEvent) { &on_closed_dialogue_ui }, 1, zox_window_dialogue).x;

    // TODO: Spawn Body Here instead
    zox_set(e, Color, { window_fill }); // (color) { 23, 23, 23, 210 } });

    // add speech text
    LayoutParentData parent_data = (LayoutParentData) { .e = e };

    SpawnTextData speech_text_data = {
        .text = "",
        .font_size = font_size,
        .font_resolution = font_size,
        .font_thickness = 8,
        .font_outline_thickness = 1,
        .font_fill_color = window_outline,
        .font_outline_color = window_outline,
        .margins = window_data.header_padding,
    };

    SpawnZext speech_text_data2 = {
        .canvas = canvas_data,
        .parent = parent_data,
        .zext = speech_text_data,
        .element = {
            .prefab = prefab_zext,
            .anchor = float2_half,
        },
    };

    entity text = spawn_zext(world, speech_text_data2);
    zox_set_parent(world, text, e);

    // add confirm button at bottom right
    // Spawn a small button per choice, for max choices, then enable disable them after text finishes animating
    SpawnTextData button_text_data = speech_text_data;
    button_text_data.text = "Z";
    int button_size = button_text_data.font_size + 8;
    int button_padding = 24;
    ElementSpawnData button_data = (ElementSpawnData) {
        .prefab = prefab_button,
        .anchor = (float2) { 1.0f, 0.0f },
        .position = (int2) {
            -button_size - button_padding,
            button_size + button_padding
        }
    };
    SpawnButtonData button_data2 = (SpawnButtonData) {
        .prefab_zext = prefab_zext,
        .fill = button_fill,
        .outline = button_outline
    };

    entity button = spawn_button(world, canvas_data, parent_data, button_data, button_text_data, button_data2);
    zox_set_parent(world, button, e);
    zox_set(button, ClickEvent, { &on_click_dialogue_button });

    return e;
}
