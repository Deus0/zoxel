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
        const int4 screen = get_element_pointer_bounds(world, &viewport, ui, zox_getv(ui, Position2), zox_getv(ui, LayoutSize));
        const int2 canvas_size = zox_getv(canvas, LayoutSize);
        // the screen's pixels to its canvas's: split screen's second viewport starts part way along (its tooltips sat
        //  that far off)
        const float2 scale = { (float) canvas_size.x / viewport.size.x, (float) canvas_size.y / viewport.size.y };
        const int4 bounds = {
            (int) ((screen.x - viewport.position.x) * scale.x), (int) ((screen.y - viewport.position.x) * scale.x),
            (int) ((screen.z - viewport.position.y) * scale.y), (int) ((screen.w - viewport.position.y) * scale.y) };
        // its top right corner is its position (MeshAlignment top right)
        int2 output = { bounds.y, bounds.z - tooltip_navigated_gap };
        // kept on its canvas: past its left edge it moves right, past its right edge left (a ui off the canvas's edge),
        //  past its bottom above the ui, then never past its top (split screen's narrow canvases cut it off)
        const int2 size = zox_getv(e, LayoutSize);
        if (output.x - size.x < 0) {
            output.x = size.x;
        }
        if (output.x > canvas_size.x) {
            output.x = canvas_size.x;
        }
        if (output.y - size.y < 0) {
            output.y = bounds.w + tooltip_navigated_gap + size.y;
        }
        if (output.y > canvas_size.y) {
            output.y = canvas_size.y;
        }
        output.x -= anchor->value.x * canvas_size.x;
        output.y -= anchor->value.y * canvas_size.y;
        if (!int2_equals(position->value, output)) {
            position->value = output;
        }
    }
    zox_sys_on_end();
} zoxd_system(tooltip_navigated_system);
