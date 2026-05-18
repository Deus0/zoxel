// TODO: Spawn basic Window + Speech Text + Confirm Button
entity spawn_dialogue_ui(ecs* world, entity prefab, entity canvas, entity character, entity target) {
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);
    int2 window_size = (int2) { 940, 200 };
    float2 position_anchor = (float2) { 0.5f, 0.74f };
    const char* header_text = zox_valid(target) ? zox_gett_value(target, ZoxName) : zox_gett_value(character, ZoxName);    // "Dialogue";
    byte header_font_size = 7 * ui_scale;
    byte2 header_padding = byte2_single(4 * ui_scale);
    int2 header_size = calculate_header_size(strlen(header_text), header_font_size, header_padding);
    SpawnWindow2 window_data = {
        .header_text = header_text,
        .header_font_size = header_font_size,
        .header_padding = header_padding,
    };
    LayoutParentData canvas_data = (LayoutParentData) {
        .e = canvas,
        .size = canvas_size
    };
    ElementSpawnData element_data = {
        .prefab = prefab,
        .size = window_size,
        .anchor = position_anchor,
    };
    entity e = spawn_window2(world, canvas_data, canvas_data, element_data, window_data, (ClickEvent) { &on_closed_dialogue_ui }, 1, 0).x;
    // zox_add_tag(e, DialogueUI);
    entity body;
    {
        // Spawn Body
        entity prefab = prefab_body; // prefab_element_textured;
        int2 body_size = int2_sub(window_size, (int2) { 0, header_size.y });
        int2 body_position = (int2) { 0, -header_size.y / 2 };
        body = spawn_uic(world, prefab, e, float2_half, body_position, body_size, body_size, window_fill, window_outline);
    }
    // Speech Text
    {
        // TODO: Can we enable 2 lines here?
        // add speech text
        entity prefab = prefab_zext;
        entity parent = body;
        const char* text = "";
        // entity canvas = canvas_data.e;
        float2 position_anchor = float2_half;
        byte alignment = 0;
        int2 position = int2_zero;
        byte font_size = 6 * ui_scale;
        byte2 padding = (byte2) { 4 * ui_scale, 2 * ui_scale };
        entity e2 = spawn_text_new(world, prefab, parent, position, position_anchor, font_size, alignment, padding, text, button_font_fill, button_font_outline);
        zox_set(e, DialogueTextLink, { e2 });
    }
    // add confirm button at bottom right
    // Spawn a small button per choice, for max choices, then enable disable them after text finishes animating
    {
        entity parent = body;
        byte font_size = 6 * ui_scale;
        int button_padding = 8 * ui_scale;
        LayoutParentData parent_data = (LayoutParentData) { .e = parent };
        SpawnTextData bdata = {
            .text = "Next",
            .font_size = font_size,
            .font_resolution = font_size,
            .font_thickness = 8,
            .font_outline_thickness = 1,
            .font_fill_color = button_font_fill,
            .font_outline_color = button_font_outline,
            .margins = window_data.header_padding,
        };
        ElementSpawnData button_data = (ElementSpawnData) {
            .prefab = prefab_button,
            .anchor = (float2) { 1, 0.5f }, // 0.5f },
            .position = (int2) { -(font_size + 2 * button_padding) * 1, 0 }
        };
        SpawnButtonData button_data2 = (SpawnButtonData) {
            .prefab_zext = prefab_zext,
            .fill = button_fill,
            .outline = button_outline
        };
        entity button = spawn_button(world, canvas_data, parent_data, button_data, bdata, button_data2);
        zox_set(button, ClickEvent, { &on_click_dialogue_button });
    }
    return e;
}
