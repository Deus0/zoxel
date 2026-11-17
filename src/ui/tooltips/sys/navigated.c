// A 2D tooltip while the player navigates (a gamepad, keyboard navigation): under the ui it shows, its top right corner
//  at the ui's bottom right, not at the mouse (the mouse is still: the tooltip sat where it last was, a corner);
//  with the mouse it follows the mouse (mouse_element_system, zoxp_update: this runs after it, zoxp_post_update). 3D windows' tooltips: follow3.c
#define tooltip_navigated_gap 2   // pixels under the ui

void tooltip_navigated_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Anchor);
    zox_sys_out(LayoutPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Anchor, anchor);
        zox_sys_o(LayoutPosition, position);
        const entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        const entity player = zox_valid(canvas) ? zox_get_link(world, canvas, PlayerLink) : 0;
        if (!zox_valid(player) || !zox_has(player, DeviceMode)) {
            continue;
        }
        const byte mode = zox_getv(player, DeviceMode);
        const byte navigating = mode == zox_device_mode_gamepad ||
            (keyboard_navigation_mode && mode == zox_device_mode_keyboardmouse);
        const entity ui = zox_get_link(world, e, TooltipLink);
        if (!navigating || !zox_valid(ui) || !zox_has(ui, Position2) || !zox_has(ui, LayoutSize)) {
            continue;
        }
        CanvasViewport viewport;
        if (!get_canvas_viewport(world, canvas, &viewport)) {
            continue;
        }
        const int4 bounds = get_element_pointer_bounds(world, &viewport, ui, zox_getv(ui, Position2), zox_getv(ui, LayoutSize));
        const int2 canvas_size = zox_getv(canvas, LayoutSize);
        // its top right corner lands half its size past its position (beside a mouse's cursor): taken off
        const int2 size = zox_getv(e, LayoutSize);
        int2 output = { bounds.y - size.x / 2, bounds.z - tooltip_navigated_gap - size.y / 2 };
        output.x -= anchor->value.x * canvas_size.x;
        output.y -= anchor->value.y * canvas_size.y;
        if (!int2_equals(position->value, output)) {
            position->value = output;
            zox_add(e, LayoutPositionDirty);
        }
    }
    zox_sys_on_end();
} zoxd_system(tooltip_navigated_system);
