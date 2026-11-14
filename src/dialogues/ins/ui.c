// TODO: Spawn basic Window + Speech Text + Confirm Button
entity spawn_dialogue_ui(
    ecs* world,
    entity canvas,
    entity character,
    entity target)
{
    int2 canvas_size = zox_getv(canvas, LayoutSize);
    int2 size = (int2) {
        canvas_size.x * 0.69f,
        canvas_size.y * 0.17f
    };
    byte header_font_size = 14 * ui_scale;
    byte speech_font_size = 9 * ui_scale;
    byte button_font_size = 12 * ui_scale;
    // its footer's height, its buttons on it (spawn_window_footer)
    const byte2 button_padding = byte2_single(4 * ui_scale);
    const int footer_margin = 2 * ui_scale;
    const int footer_height = button_font_size + button_padding.y * 2 + footer_margin * 2;
    // at least its footer and two lines of text tall (its margins and padding round them), on a small screen
    const int text_min_height = 2 * (speech_font_size + default_line_padding) + (6 + 2) * 2 * ui_scale;
    size.y = int_max(size.y, footer_height + text_min_height);
    // int2 canvas_size = zox_getv(canvas, LayoutSize);
    float2 position_anchor = (float2) { 0.5f, 0.84f };
    const char* header_text =
        zox_valid(target) ?
            zox_getv(target, ZoxName) :
            zox_valid(character) ?
                zox_getv(character, ZoxName) :
                "???";
    byte2 header_padding = (byte2) { 10 * ui_scale, 4 * ui_scale };
    // its body the text, a footer under it for its buttons (a strip of its own, they don't overlap)
    size.y -= footer_height;
    entity footer = 0;
    entity3 e3 = spawn_window_footer(
        world,
        prefab_window,
        prefab_body,
        header_text,
        canvas,
        int2_zero,
        size,
        position_anchor,
        header_font_size,
        header_padding,
        &on_closed_dialogue_ui,
        footer_height,
        &footer);
    entity e = e3.x;
    entity body = e3.z;
    zox_add(e, DialogueUI);
    zox_add(e, NavigationWindow);
    // Speech Text
    {
        entity parent = body;
        const char* text = "";
        float2 position_anchor = float2_centre;
        int2 text_margins = {
            6 * ui_scale,
            6 * ui_scale
        };
        int2 position = {
            -size.x / 2,
            size.y / 2
        };
        position.x += text_margins.x;
        position.y -= text_margins.y;
        byte alignment = zox_alignment_top_left;
        byte2 padding = (byte2) {
            4 * ui_scale,
            2 * ui_scale
        };
        // TODO: Text should be positioned based on alignment
        entity text_ui = spawn_text(
            world,
            prefab_dialogue_text,
            parent,
            position,
            position_anchor,
            speech_font_size,
            alignment,
            padding,
            text,
            button_font_fill,
            button_font_outline);
        zox_set_unique_name(text_ui, "dialogue_text");
        // inside the margins, lines wrap to it (dialogue_speech_system)
        zox_setv(text_ui, LayoutSize, int2_sub(size, int2_multiply1(text_margins, 2)));
        zox_link(world, e, DialogueLabel, text_ui);
    }
    // Next, on the footer's right
    // TODO: a small button per choice, enabled after the text finishes animating
    {
        entity2 e2 = spawn_button(
            world,
            prefab_button,
            footer,
            "Next",
            (int2) { -footer_margin, 0 },
            int2_zero,
            (float2) { 1, 0.5f },
            zox_alignment_right,
            button_font_size,
            button_padding,
            button_fill,
            button_outline,
            button_font_fill,
            button_font_outline);
        zox_add(e2.x, DialogueButton);
        zox_setv(e2.x, MeshAlignment, zox_alignment_right);
        zox_setv(e2.x, ClickEvent, &on_click_dialogue_button);
    }
    return e;
}
