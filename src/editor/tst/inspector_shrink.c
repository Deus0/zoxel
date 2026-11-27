// tst: inspector_shrink, a docked inspector narrower than its widest row keeps its width (rows scroll sideways)
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
        int2 size = zox_getv(inspector, LayoutSize);
        width = size.x = 64;
        zox_setv(inspector, LayoutSize, size);
        zox_add(inspector, Dirty);
    } else if (step < 30) {
        // its rows lay out, its window fits them again
    } else {
        zox_tst_check(zox_getv(inspector, LayoutSize).x == width, "the docked inspector grew to [%i], not [%i] wide",
            zox_getv(inspector, LayoutSize).x, width);
        return 1;
    }
    step++;
    return 0;
}
