// tst: hierarchy_prefabs, the hierarchy header's p toggle shows prefab children, and hides them again
static byte zox_tst_hierarchy_prefabs_ready(ecs* world) {
    static entity window, root, button;
    static int step;
    const entity canvas = get_editor_canvas(world);
    if (!canvas) {
        return 0;
    }
    const entity list = window ? zox_tst_editor_list(world, window) : 0;
    const int rows = window ? zox_tst_hierarchy_count(world, window) : 0;
    if (step == 0) {
        root = zox_new();
        zox_set_parent(world, zox_new(), root);
        const entity prefab = zox_new();
        zox_add_id(prefab, EcsPrefab);
        zox_set_parent(world, prefab, root);
        zox_defer_begin();
        window = spawn_window_hierarchy(world, canvas, 0, root);
        zox_defer_end();
    } else if (step == 1) {
        zox_tst_waiting = "the root's row";
        if (rows != 1) {
            return 0;
        }
        unfold_toggle(world, list, root);
    } else if (step == 2) {
        zox_tst_waiting = "the root and its child, no prefab";
        if (rows != 2) {
            return 0;
        }
        const entity header = zox_get_child_by_id(world, window, zox_id(Header));
        button = zox_get_child_by_id(world, header, zox_id(HeaderButton));
        zox_tst_check(button, "the hierarchy header has no prefabs toggle");
        if (!button) {
            return 1;
        }
        zox_getv(button, ClickEvent)(world, (ClickEventData) { .clicked = button });
    } else if (step == 3) {
        zox_tst_waiting = "the prefab child to show";
        if (rows != 3) {
            return 0;
        }
        zox_getv(button, ClickEvent)(world, (ClickEventData) { .clicked = button });
    } else {
        zox_tst_waiting = "the prefab child to hide again";
        return rows == 2;
    }
    step++;
    return 0;
}
