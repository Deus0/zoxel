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
    // zox_geter_value(canvas, LayoutSize, int2, canvas_size);
    float2 position_anchor = (float2) { 0.5f, 0.84f };
    const char* header_text =
        zox_valid(target) ?
            zox_getv(target, ZoxName) :
            zox_valid(character) ?
                zox_getv(character, ZoxName) :
                "???";
    byte2 header_padding = (byte2) { 10 * ui_scale, 4 * ui_scale };
    entity3 e3 = spawn_window(
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
        &on_closed_dialogue_ui);
    entity e = e3.x;
    entity body = e3.z;
    zox_add(e, DialogueUI);
    zox_add(e, NavigationWindow);
    // Speech Text
    {
        // TODO: Can we enable 2 lines here?
        // add speech text
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
        zox_setv(text_ui, LayoutSize, size);
        zox_link(world, e, DialogueLabel, text_ui);
    }
    // NOTE: A Confirm button at bottom right
    // Spawn a small button per choice, for max choices, then enable disable them after text finishes animating
    {
        entity parent = body;
        byte2 button_padding = byte2_single(4 * ui_scale);
        float2 position_anchor = float2_bottom_right; //  (float2) { 1, 0 };
        entity2 e2 = spawn_button(
            world,
            prefab_button,
            parent,
            "Next",
            int2_zero,
            int2_zero,
            position_anchor,
            zox_alignment_bottom_right,
            button_font_size,
            button_padding,
            button_fill,
            button_outline,
            button_font_fill,
            button_font_outline);
        zox_add(e2.x, DialogueButton);
        zox_setv(e2.x, MeshAlignment, zox_alignment_bottom_right);
        zox_setv(e2.x, ClickEvent, &on_click_dialogue_button);
    }
    return e;
}
