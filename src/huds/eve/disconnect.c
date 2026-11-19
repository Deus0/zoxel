// the disconnect screen: a player's gamepad unplugged (DeviceLost, on_gamepad_disconnected), its screen says so, in
//  its middle, until a gamepad reaches it again (reclaim_gamepads) or it uses another device (the keyboard)
void disconnect_screen_show(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity canvas = zox_get_link(world, e, CanvasLink);
        if (!zox_valid(canvas) || zox_valid(zox_get_child_by_id(world, canvas, zox_id(DisconnectUI)))) {
            continue;
        }
        // as a toast (a frame, its text over it, the overlay layer), sized to fit its screen (split, half of it)
        const char* text = "Gamepad disconnected";
        const int length = (int) strlen(text);
        const byte2 padding = byte2_single(6 * ui_scale);
        const int width = zox_getv(canvas, LayoutSize).x;
        byte font_size = 12 * ui_scale;
        while (font_size > 4 && length * font_size + padding.x * 2 > width * 9 / 10) {
            font_size--;
        }
        const int2 size = { length * font_size + padding.x * 2, font_size + padding.y * 2 };
        const entity frame = spawn_uic(world, prefab_element_frame, canvas, float2_half, int2_zero, size, size,
            header_fill, header_outline);
        zox_setv(frame, Layer, get_game_overlay_layer());
        zox_set_unique_name(frame, "disconnect_screen");
        zox_add(frame, DisconnectUI);
        const entity label = spawn_text(world, prefab_text, frame, int2_zero, float2_half, font_size, zox_alignment_centre,
            byte2_zero, text, header_font_fill, header_font_outline);
        zox_setv(label, Layer, get_game_overlay_layer() + 1);
    }
}

void disconnect_screen_hide(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity canvas = zox_get_link(world, e, CanvasLink);
        const entity label = zox_valid(canvas) ? zox_get_child_by_id(world, canvas, zox_id(DisconnectUI)) : 0;
        if (zox_valid(label)) {
            zox_delete(label);
        }
    }
}
