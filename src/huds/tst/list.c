// tst: list_order, a list keeps its elements in order (ChildIndex) while they're hovered and scrolled
//  - children iterate by table, and hovering moves elements between tables (Select tags),
//      so a list laid out by iteration order shuffled
//  - hovers every shown options row with the fake mouse, scrolls down and back up, then checks:
//      shown rows are exactly [ListStart, ListStart + ListVisible) by ChildIndex,
//      and they're laid out top to bottom in ChildIndex order
//  - run: bash bsh/tst.sh list_order
#define zox_tst_list_hold_frames 3
int zox_tst_list_stage = 0;
int zox_tst_list_frames = 0;
int zox_tst_list_hover = 0;
entity zox_tst_list_view = 0;
entity zox_tst_list_list = 0;

// checks the list's shown rows against ListStart and ChildIndex order
void zox_tst_list_check_order(ecs* world, const char* when) {
    const entity list = zox_tst_list_list;
    const int start = zox_getv(list, ListStart);
    const int visible = zox_getv(list, ListVisible);
    int shown = 0;
    int wrong_shown = 0;
    int misordered = 0;
    // shown rows by ChildIndex, their y
    int ys[256];
    byte has_y[256];
    memset(has_y, 0, sizeof(has_y));
    iter it2 = zox_children(world, list);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity row = it2.entities[j];
            if (!zox_has(row, ChildIndex) || !zox_has(row, RenderDisabled)) {
                continue;
            }
            const int index = zox_getv(row, ChildIndex);
            const byte should_show = index >= start && index < start + visible;
            const byte is_shown = !zox_getv(row, RenderDisabled);
            wrong_shown += should_show != is_shown;
            if (is_shown && index < 256) {
                shown++;
                ys[index] = zox_getv(row, LayoutPosition).y;
                has_y[index] = 1;
            }
        }
    }
    int last_y = 0;
    byte has_last = 0;
    for (int index = 0; index < 256; index++) {
        if (!has_y[index]) {
            continue;
        }
        if (has_last && ys[index] >= last_y) {
            misordered++;
        }
        last_y = ys[index];
        has_last = 1;
    }
    zox_tst_check(wrong_shown == 0, "%s: [%i] rows shown or hidden wrong for start [%i]", when, wrong_shown, start);
    zox_tst_check(misordered == 0, "%s: [%i] of [%i] shown rows out of ChildIndex order", when, misordered, shown);
}

byte zox_tst_list_order_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_list_frames++;
    const entity main_menu = zox_tst_menu(world, canvas, zox_id(MenuMain));
    const entity options = zox_tst_menu(world, canvas, zox_id(MenuOptions));
    switch (zox_tst_list_stage) {
        case 0:
            zox_tst_waiting = "a mouse click to open the main menu";
            zox_tst_mouse((int2) { 100, 100 }, (zox_tst_list_frames / 4) % 2);
            if (!main_menu) {
                return 0;
            }
            break;
        case 1:
            zox_tst_mouse((int2) { 100, 100 }, 0);
            if (zox_tst_list_frames < 4) {
                return 0;
            }
            if (!zox_tst_menus_click(world, player, main_menu, &button_event_menu_options, "options")) {
                return 1;
            }
            break;
        case 2:
            zox_tst_waiting = "the options menu's layout";
            if (!options || zox_tst_list_frames < 10) {
                return 0;
            }
            zox_tst_list_view = zox_get_link(world, options, ScrollviewLink);
            zox_tst_list_list = zox_valid(zox_tst_list_view) ? zox_get_link(world, zox_tst_list_view, ListUILink) : 0;
            if (!zox_tst_list_list) {
                zox_tst_check(0, "options has no list");
                return 1;
            }
            zox_tst_list_check_order(world, "opened");
            break;
        case 3: {
            // hovers each shown row in turn (rows move tables as they're selected / deselected)
            zox_tst_waiting = "hovering the rows";
            if (zox_tst_list_frames < zox_tst_list_hold_frames) {
                return 0;
            }
            zox_tst_list_frames = 0;
            const int start = zox_getv(zox_tst_list_list, ListStart);
            entity row = 0;
            iter it2 = zox_children(world, zox_tst_list_list);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count && !row; j++) {
                    if (zox_has(it2.entities[j], ChildIndex) && (int) zox_getv(it2.entities[j], ChildIndex) == start + zox_tst_list_hover) {
                        row = it2.entities[j];
                    }
                }
            }
            int2 position;
            if (row && zox_tst_ui_pointer_position(world, canvas, row, &position)) {
                zox_tst_mouse(position, 0);
            }
            zox_tst_list_hover++;
            if (zox_tst_list_hover < zox_getv(zox_tst_list_list, ListVisible)) {
                return 0;
            }
            zox_tst_list_check_order(world, "hovered");
            break;
        }
        case 4:
        case 5:
            // scrolls down then back up a notch, relaying out the hovered rows
            zox_tst_waiting = "scrolling";
            zox_tst_mouse_wheel(zox_tst_list_frames == 1 ? (zox_tst_list_stage == 4 ? -1 : 1) : 0);
            if (zox_tst_list_frames < 6) {
                return 0;
            }
            zox_tst_list_check_order(world, zox_tst_list_stage == 4 ? "scrolled down" : "scrolled back up");
            if (zox_tst_list_stage == 5) {
                zox_tst_mouse_end();
                return 1;
            }
            break;
    }
    zox_tst_list_stage++;
    zox_tst_list_frames = 0;
    return 0;
}

void zox_tst_list_order_check(ecs* world) {
    (void) world;
    zox_tst_mouse_end();
}
