// tst: hierarchy_spawned, a child spawned under an unfolded row shows without reopening (hierarchy_child_event)
static byte zox_tst_hierarchy_spawned_ready(ecs* world) {
    static entity window, root, child;
    static int step;
    const entity canvas = get_editor_canvas(world);
    if (!canvas) {
        return 0;
    }
        const entity list = window ? zox_tst_editor_list(world, window) : 0;
    if (step == 0) {
        root = zox_new();
        zox_set_parent(world, zox_new(), root);
        zox_defer_begin();
        window = spawn_window_hierarchy(world, canvas, 0, root);
        zox_defer_end();
    } else if (step == 1) {
        unfold_toggle(world, list, root);
        zox_tst_check(zox_tst_hierarchy_count(world, window) == 2, "expected the root and its child");
    } else if (step < 40) {
        // past the window's layout settling (it refreshes the list for its first frames)
    } else if (step == 40) {
        child = zox_new();
        zox_set_parent(world, child, root);
        step++;
        return 0;
    } else {
        zox_tst_waiting = "the spawned child's row";
        entity slots[UINT8_MAX + 1];
        const int count = get_list_children_ordered(world, list, slots, UINT8_MAX + 1);
        for (int i = 0; i < count; i++) {
            if (zox_get_link(world, slots[i], RowTargetLink) == child) {
                return 1;
            }
        }
        zox_tst_check(++step < 160, "the spawned child never showed");
        return 0;
    }
    step++;
    return 0;
}
