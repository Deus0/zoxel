// the held item's icon (DataMouse) for a player not on the mouse (a navigator: a gamepad, a headset): top right of
//  its screen, not on a pointer it doesn't move; on the mouse again for the first player back on it
#define data_mouse_corner_margin 8   // pixels from the corner, by ui_scale

void data_mouse_corner_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Anchor);
    zox_sys_in(LayoutSize);
    zox_sys_out(LayoutPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Anchor, anchor);
        zox_sys_i(LayoutSize, size);
        zox_sys_o(LayoutPosition, position);
        const entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        const entity player = zox_valid(canvas) ? zox_get_link(world, canvas, PlayerLink) : 0;
        if (!zox_valid(player)) {
            continue;
        }
        const byte mode = zox_has(player, DeviceMode) ? zox_getv(player, DeviceMode) : 0;
        const byte on_mouse = mode == zox_device_mode_keyboardmouse && player == zox_players[0] && zox_valid(local_mouse);
        if (on_mouse) {
            // back on the mouse: mouse_element_system follows its pointer
            if (zox_get_link(world, e, DeviceLink) != local_mouse) {
                zox_set_link(world, e, DeviceLink, local_mouse);
                zox_set_link(world, e, ZeviceLink, 0);
            }
            continue;
        }
        if (zox_valid(zox_get_link(world, e, DeviceLink)) || zox_valid(zox_get_link(world, e, ZeviceLink))) {
            zox_set_link(world, e, DeviceLink, 0);
            zox_set_link(world, e, ZeviceLink, 0);
        }
        // its middle, from its anchor (pointer space: bottom left origin)
        const int2 canvas_size = zox_getv(canvas, LayoutSize);
        const int margin = data_mouse_corner_margin * ui_scale;
        const int2 output = (int2) {
            canvas_size.x - margin - size->value.x / 2 - (int) (anchor->value.x * canvas_size.x),
            canvas_size.y - margin - size->value.y / 2 - (int) (anchor->value.y * canvas_size.y)
        };
        if (!int2_equals(position->value, output)) {
            position->value = output;
        }
    }
    zox_sys_on_end();
} zoxd_system(data_mouse_corner_system);
