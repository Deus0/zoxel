// tst: inspector_tooltip, hovering an inspector row shows its component's name and type in the canvas tooltip
static byte zox_tst_inspector_tooltip_ready(ecs* world) {
    static entity inspector, row;
    static int step;
    static int2 point;
    zox_tst_progress(step);
    if (step == 0 && !zox_tst_ui_fixture_canvas(world)) {
        return 0;   // the mouse woken, the canvas cleared
    }
    const entity canvas = get_editor_canvas(world);
    if (step == 0) {
        spawn_inspector(world, canvas, dbg_player, canvas);
    } else if (step == 1) {
        zox_tst_waiting = "a component row of the inspector";
        inspector = get_canvas_inspector(world, canvas);
        const entity list = inspector ? get_editor_window_list(world, inspector) : 0;
        row = 0;
        if (list) {
            iter it = zox_children(world, list);
            while (zox_children_next(it)) {
                for (int i = 0; i < it.count; i++) {
                    const entity e = it.entities[i];
                    if (!row && zox_has(e, ComponentTarget) && zox_getv(e, ComponentTarget) && !zox_getv(e, RenderDisabled)) {
                        row = e;
                    }
                }
            }
        }
        if (!row || !zox_tst_ui_pointer_position(world, canvas, row, &point)) {
            return 0;
        }
        zox_tst_mouse(point, 0);
    } else {
        zox_tst_waiting = "the row's tooltip";
        zox_tst_mouse(point, 0);
        const entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
        if (!tooltip || zox_getv(tooltip, RenderDisabled)) {
            return 0;
        }
        // the row under the pointer (rows can settle after the point was taken)
        const entity hovered = zox_tst_mouse_target(world, dbg_player);
        if (!zox_has(hovered, ComponentTarget) || !zox_getv(hovered, ComponentTarget)) {
            return 0;
        }
        char* text = get_zext_text(zox_get(tooltip, TextData));
        const char* name = zox_get_name(zox_getv(hovered, ComponentTarget));
        const byte named = text && name && strstr(text, name);
        free(text);
        return named;
    }
    step++;
    return 0;
}
