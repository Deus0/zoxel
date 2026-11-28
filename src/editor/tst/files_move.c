// tst: files_move, a floating files window moved each frame keeps its rows shown, on it and unclipped
static byte zox_tst_files_move_ready(ecs* world) {
    static entity window, list, first;
    static int step;
    static float2 offset;
    const entity canvas = get_editor_canvas(world);
    if (!canvas) {
        return 0;
    }
    if (step == 0) {
        window = spawn_window_files(world, canvas, dbg_player, 0);
    } else if (step < 20) {
        if (step == 10 && zox_has(window, Docked)) {
            dock_undock(world, window);
        }
    } else {
        if (!list) {
            list = get_editor_window_list(world, window);
        }
        entity rows[UINT8_MAX + 1];
        const int count = get_list_children_ordered(world, list, rows, UINT8_MAX + 1);
        const entity row = rows[0];
        const float2 o = float2_sub(zox_getv(row, Position2), zox_getv(window, Position2));
        (void) count;
        if (step == 21) {
            offset = o;
            first = row;
        } else if (step > 21) {
            zox_tst_check(row == first, "the moved window respawned its rows");
            zox_tst_check(float2_equals(o, offset), "a row lags its moving window");
            if (row != first || !float2_equals(o, offset)) {
                return 1;
            }
        }
        zox_muter(window, LayoutPosition, position);
        position->value.x += 3;
        if (step > 60) {
            return 1;
        }
    }
    step++;
    return 0;
}
