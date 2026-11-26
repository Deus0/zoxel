static byte zox_tst_hierarchy_big_ready(ecs* world) {
    static entity window, root, last, first_slot;
    static int step;
    const entity canvas = get_editor_canvas(world);
    if (!canvas) {
        return 0;
    }
    zox_tst_progress(step);
    if (step == 0) {
        root = zox_new();
        zox_add_ordered_children(root);
        for (int i = 0; i < 3000; i++) {
            last = zox_new();
            zox_set_parent(world, last, root);
        }
        zox_defer_begin();
        window = spawn_window_hierarchy(world, canvas, 0, root);
        zox_defer_end();
    } else {
        const entity list = zox_tst_editor_list(world, window);
        entity slots[UINT8_MAX + 1];
        const int count = get_list_children_ordered(world, list, slots, UINT8_MAX + 1);
        const int visible = zox_getv(list, ListVisible);
        zox_tst_check(count == visible + 1, "expected %i slots, got %i", visible + 1, count);
        if (step == 1) {
            first_slot = slots[0];
            const double start = get_time_ms();
            unfold_toggle(world, list, root);
            const double elapsed = get_time_ms() - start;
            zox_log("[tst] hierarchy_big refreshed 3001 rows in %.3f ms", elapsed);
            zox_tst_check(elapsed < 100.0, "unfold refresh took %.3f ms", elapsed);
            zox_tst_check(zox_tst_hierarchy_count(world, window) == 3001, "expected 3001 walk rows");
            scroll_virtual_list_fraction(world, list, 1.0f);
        } else if (step == 2) {
            zox_tst_check(slots[0] == first_slot, "scroll replaced the slots");
            zox_tst_check(zox_get_link(world, slots[visible - 1], RowTargetLink) == last, "last child missing at end");
            unfold_toggle(world, list, root);
        } else {
            zox_tst_check(zox_tst_hierarchy_count(world, window) == 1, "expected one folded row");
            zox_tst_check(zox_get_link(world, slots[0], RowTargetLink) == root, "folded root missing");
            zox_tst_check(zox_getv(slots[1], RenderDisabled), "unlinked slot still visible");
            return 1;
        }
    }
    step++;
    return 0;
}
