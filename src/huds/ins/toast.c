// a toast: a short message in a frame high on the screen, it pops in, stays toast_seconds, then pops out
//  - a new toast replaces the one there
const double toast_seconds = 3;

void toast_end(ecs* world, entity e) {
    if (zox_valid(e) && !zox_has(e, UIClosing)) {
        ui_pop_out(world, e);
    }
}

entity spawn_toast(ecs* world, entity canvas, const char* text) {
    if (!zox_valid(canvas)) {
        return 0;
    }
    toast_end(world, zox_get_child_by_id(world, canvas, zox_id(Toast)));
    const byte font_size = 12 * ui_scale;
    const byte2 padding = byte2_single(6 * ui_scale);
    const int2 size = { (int) strlen(text) * font_size + padding.x * 2, font_size + padding.y * 2 };
    // under the hud along the top, over it (the overlay layer, its text above)
    const entity e = spawn_uic(world, prefab_element_frame, canvas, (float2) { 0.5f, 0.7f }, int2_zero, size, size,
        header_fill, header_outline);
    zox_setv(e, Layer, get_game_overlay_layer());
    zox_set_unique_name(e, "toast");
    zox_add(e, Toast);
    const entity label = spawn_text(world, prefab_text, e, int2_zero, float2_half, font_size, zox_alignment_centre,
        byte2_zero, text, header_font_fill, header_font_outline);
    zox_setv(label, Layer, get_game_overlay_layer() + 1);
    ui_pop_in(world, e);
    delay_event(world, &toast_end, e, toast_seconds);
    return e;
}
