// tst: dropdown, opens on click, picking an option selects it and closes, a click outside closes it (ins/dropdown.c)
static byte zox_tst_dropdown_ready(ecs* world) {
    static int step = 0;
    static entity canvas, dropdown;
    zox_tst_progress(step);
    if (step == 0) {
        zox_tst_waiting = "the fixture canvas";
        canvas = zox_tst_ui_fixture_canvas(world);
        if (!canvas) {
            return 0;
        }
        const char* options[] = { "name", "tag", "type" };
        dropdown = spawn_dropdown(world, canvas, canvas, (int2) { 0, 0 }, (int2) { 120, 24 }, float2_half,
            options, 3, 0, 12, 32);
        step = 1;
    } else if (step == 1) {
        zox_tst_check(!zox_get_child_by_id(world, dropdown, zox_id(DropdownPopup)), "closed at spawn");
        zox_tst_click(world, dbg_player, dropdown);
        step = 2;
    } else if (step == 2) {
        zox_tst_waiting = "the popup to open";
        entity popup = zox_get_child_by_id(world, dropdown, zox_id(DropdownPopup));
        if (!popup) {
            return 0;
        }
        // the options: the popup's children (iterated by table)
        entity options[8];
        int count = 0;
        iter it2 = zox_children(world, popup);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count && count < 8; j++) {
                if (zox_has(it2.entities[j], ClickEvent)) {
                    options[count++] = it2.entities[j];
                }
            }
        }
        zox_tst_check(count == 3, "popup has [%i] options, not 3", count);
        if (count < 2) {
            return 1;
        }
        zox_tst_click(world, dbg_player, options[1]);
        step = 3;
    } else if (step == 3) {
        zox_tst_waiting = "the popup to close";
        entity popup = zox_get_child_by_id(world, dropdown, zox_id(DropdownPopup));
        if (popup) {
            return 0;
        }
        zox_tst_check(zox_getv(dropdown, DropdownIndex) == 1, "selected index is 1, not %i", zox_getv(dropdown, DropdownIndex));
        zox_tst_click(world, dbg_player, dropdown);
        step = 4;
    } else if (step == 4) {
        zox_tst_waiting = "the popup to reopen";
        entity popup = zox_get_child_by_id(world, dropdown, zox_id(DropdownPopup));
        if (!popup) {
            return 0;
        }
        // Move mouse outside the popup and release
        zox_tst_mouse((int2) { 4, 4 }, 0);
        step = 5;
    } else if (step == 5) {
        // Need to wait a frame for the press to be processed
        zox_tst_mouse((int2) { 4, 4 }, 1);
        step = 6;
    } else if (step == 6) {
        zox_tst_mouse((int2) { 4, 4 }, 0);
        step = 7;
    } else if (step == 7) {
        static int frames = 0;
        frames++;
        if (frames > 60) {
            zox_tst_check(0, "popup did not close after clicking outside");
            return 1;
        }
        entity popup = zox_get_child_by_id(world, dropdown, zox_id(DropdownPopup));
        if (popup) {
            return 0;
        }
        zox_tst_check(zox_getv(dropdown, DropdownIndex) == 1, "index is still 1 after clicking outside, not %i", zox_getv(dropdown, DropdownIndex));
        return 1;
    }
    return 0;
}