// tst: inspector_shrink, a docked inspector fits under its header once its parts are there (they arrive after its
//  first layout), and narrower than its widest row keeps its width (rows scroll sideways)
static byte zox_tst_inspector_shrink_ready(ecs* world) {
    static entity inspector;
    static int step, width;
    const entity canvas = get_editor_canvas(world);
    if (!canvas) {
        return 0;
    }
    if (step == 0) {
        spawn_inspector(world, canvas, dbg_player, canvas);
    } else if (step == 1) {
        zox_tst_waiting = "the inspector to dock";
        inspector = get_canvas_inspector(world, canvas);
        if (!inspector || !zox_has(inspector, Docked) || zox_getv(inspector, LayoutSize).x <= 0) {
            return 0;
        }
    } else if (step < 20) {
        // its parts arrive, its layout settles
    } else if (step == 20) {
        const entity header = zox_get_child_by_id(world, inspector, zox_id(Header));
        const entity body = zox_get_child_by_id(world, inspector, zox_id(WindowBody));
        const entity view = zox_get_link(world, inspector, ScrollviewLink);
        const int body_height = zox_getv(inspector, LayoutSize).y - zox_getv(header, LayoutSize).y;
        zox_tst_check(zox_getv(body, LayoutSize).y == body_height && zox_getv(view, LayoutSize).y == body_height,
            "the inspector's body [%i] and scroll view [%i] aren't [%i] high, under its header",
            zox_getv(body, LayoutSize).y, zox_getv(view, LayoutSize).y, body_height);
        // its header's buttons right to left, none over another (the undock button spawns as it docks)
        const entity close = zox_get_child_by_id(world, header, zox_id(CloseButton));
        const entity undock = zox_get_child_by_id(world, header, zox_id(UndockButton));
        const entity search = zox_get_child_by_id(world, header, zox_id(WindowSearchButton));
        const int close_left = -zox_getv(close, LayoutSize).x;
        const int undock_right = zox_getv(undock, LayoutPosition).x + zox_getv(undock, LayoutSize).x / 2;
        const int undock_left = undock_right - zox_getv(undock, LayoutSize).x;
        const int search_right = zox_getv(search, LayoutPosition).x + zox_getv(search, LayoutSize).x / 2;
        zox_tst_check(undock_right <= close_left && search_right <= undock_left,
            "header buttons overlap: close from [%i], undock [%i to %i], search to [%i]", close_left, undock_left, undock_right, search_right);
        int2 size = zox_getv(inspector, LayoutSize);
        width = size.x = 64;
        zox_setv(inspector, LayoutSize, size);
        zox_add(inspector, Dirty);
    } else if (step < 50) {
        // its rows lay out, its window fits them again
    } else {
        zox_tst_check(zox_getv(inspector, LayoutSize).x == width, "the docked inspector grew to [%i], not [%i] wide",
            zox_getv(inspector, LayoutSize).x, width);
        return 1;
    }
    step++;
    return 0;
}
