// TODO: Spawn basic Window + Speech Text + Confirm Button
entity spawn_dialogue_ui(ecs* world, entity prefab, entity canvas, entity character, entity target) {
    // zox_geter_value(canvas, LayoutSize, int2, canvas_size);
    int2 size = (int2) { 740, 160 };
    float2 position_anchor = (float2) { 0.5f, 0.74f };
    const char* header_text = zox_valid(target) ? zox_getv(target, ZoxName) : zox_getv(character, ZoxName);
    // byte header_font_size = 7 * ui_scale;
    // byte2 header_padding = byte2_single(4 * ui_scale);
    // int2 header_size = calculate_header_size(strlen(header_text), header_font_size, header_padding);
    entity3 e3 = spawn_window(world, prefab, prefab_body, header_text, canvas, int2_zero, size, position_anchor, &on_closed_dialogue_ui);
    entity e = e3.x;
    entity body = e3.z;
    // zox_add_tag(e, DialogueUI);
    // Speech Text
    {
        // TODO: Can we enable 2 lines here?
        // add speech text
        entity prefab = prefab_text;
        entity parent = body;
        const char* text = "";
        float2 position_anchor = float2_half;
        int2 text_margins = { 6 * ui_scale, 6 * ui_scale };
        int2 position = { -size.x / 2, size.y / 2 };
        position.x += text_margins.x;
        position.y -= text_margins.y;
        byte alignment = zox_alignment_top_left;
        byte font_size = 6 * ui_scale;
        byte2 padding = (byte2) { 4 * ui_scale, 2 * ui_scale };
        // TODO: Text should be positioned based on alignment
        entity e2 = spawn_text(world, prefab, parent, position, position_anchor, font_size, alignment, padding, text, button_font_fill, button_font_outline);
        zox_set(e, DialogueTextLink, { e2 });
        zox_set(e2, LayoutSize, { size });
        // zox_set(e2, TextureSize, { size });
    }
    // NOTE: A Confirm button at bottom right
    // Spawn a small button per choice, for max choices, then enable disable them after text finishes animating
    {
        entity parent = body;
        byte font_size = 6 * ui_scale;
        byte2 button_padding = byte2_single(4 * ui_scale);
        float2 position_anchor = (float2) { 1, 0 };
        entity e2 = spawn_button(world, prefab_button, parent, "Next", int2_zero, int2_zero, position_anchor, zox_alignment_bottom_right, font_size, button_padding, button_fill, button_outline, button_font_fill, button_font_outline);
        zox_set(e2, MeshAlignment, { zox_alignment_bottom_right });
        zox_set(e2, ClickEvent, { &on_click_dialogue_button });
    }
    return e;
}
