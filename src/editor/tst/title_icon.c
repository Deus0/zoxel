// tst: title_icon, an editor window's title sits after its header icon, and hides (the icon stays) when its window
//  is too narrow to read it, showing again when widened
static byte zox_tst_title_icon_ready(ecs* world) {
    static entity window, header, icon, title;
    static int step, wait;
    const entity canvas = get_editor_canvas(world);
    if (!canvas) {
        return 0;
    }
    zox_tst_progress(step);
    if (step == 0) {
        window = spawn_window_hierarchy(world, canvas, 0, 0);
    } else if (step == 1) {
        zox_tst_waiting = "the hierarchy's header, its icon and title";
        header = zox_get_child_by_id(world, window, zox_id(Header));
        icon = header ? zox_get_child_by_id(world, header, zox_id(HeaderIcon)) : 0;
        title = header ? zox_get_child_by_id(world, header, zox_id(TextFit)) : 0;
        if (!icon || !title || !zox_has(window, Docked) || wait++ < 20) {
            return 0;
        }
        wait = 0;
        int2 size = zox_getv(window, LayoutSize);
        size.x = 120;
        zox_setv(window, LayoutSize, size);
        zox_add(window, Dirty);
    } else if (step == 2) {
        zox_tst_waiting = "the narrow window's title to hide";
        if (!zox_getv(title, RenderDisabled) && wait++ < 60) {
            return 0;
        }
        zox_tst_check(zox_getv(title, RenderDisabled), "a narrow window's title still shows at font [%i]",
            zox_getv(title, TextFontSize));
        zox_tst_check(!zox_getv(icon, RenderDisabled), "the header icon hid with the title");
        wait = 0;
        int2 size = zox_getv(window, LayoutSize);
        size.x = 360;
        zox_setv(window, LayoutSize, size);
        zox_add(window, Dirty);
    } else {
        zox_tst_waiting = "the wide window's title to show, after its icon";
        if (zox_getv(title, RenderDisabled)) {
            zox_tst_check(wait++ < 60, "a wide window's title stayed hidden");
            return wait >= 60;
        }
        const int icon_right = zox_getv(icon, LayoutPosition).x + zox_getv(icon, LayoutSize).x / 2;
        const int title_left = zox_getv(title, LayoutPosition).x - zox_getv(title, LayoutSize).x / 2;
        zox_tst_check(title_left >= icon_right, "the title [%i] starts over its icon [%i]", title_left, icon_right);
        return 1;
    }
    step++;
    return 0;
}
