// tst: scroll_wheel, scrolls the options menu's list with the fake mouse wheel and scrollbar
//  - options lists more settings than it shows, so it has a scrollbar
//  - wheel down over a list element scrolls one element per notch, the handle follows
//  - scrolling past either end clamps (to the last start, and back to 0)
//  - the wheel over empty space (not over the scrollview) doesn't scroll
//  - the wheel over the scrollview but between elements (over the window body) still scrolls
//  - dragging the handle back to the top scrolls back to 0
//  - run: bash bsh/tst.sh scroll_wheel
#define zox_tst_scroll_hold_frames 4
int zox_tst_scroll_stage = 0;
int zox_tst_scroll_frames = 0;
entity zox_tst_scroll_view = 0;
entity zox_tst_scroll_list = 0;
entity zox_tst_scroll_handle = 0;
int zox_tst_scroll_max = 0;
int2 zox_tst_scroll_on_list;
int2 zox_tst_scroll_on_gap;
int2 zox_tst_scroll_on_empty;
int zox_tst_scroll_handle_top = 0;

static inline int zox_tst_scroll_start(ecs* world) {
    return zox_getv(zox_tst_scroll_list, ListStart);
}

static inline int zox_tst_scroll_handle_y(ecs* world) {
    return zox_getv(zox_tst_scroll_handle, LayoutPosition).y;
}

// scrolls the wheel for one frame, then rests for a few, returns 1 once rested
static inline byte zox_tst_scroll_notch(int2 position, int notch) {
    zox_tst_mouse(position, 0);
    zox_tst_mouse_wheel(zox_tst_scroll_frames == 1 ? notch : 0);
    return zox_tst_scroll_frames >= zox_tst_scroll_hold_frames;
}

byte zox_tst_scroll_wheel_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_scroll_frames++;
    const entity main_menu = zox_tst_menu(world, canvas, zox_id(MenuMain));
    const entity options = zox_tst_menu(world, canvas, zox_id(MenuOptions));
    switch (zox_tst_scroll_stage) {
        case 0:
            zox_tst_waiting = "a mouse click to open the main menu";
            zox_tst_mouse((int2) { 100, 100 }, (zox_tst_scroll_frames / zox_tst_scroll_hold_frames) % 2);
            if (!main_menu) {
                return 0;
            }
            break;
        case 1:
            zox_tst_waiting = "the main menu to settle";
            zox_tst_mouse((int2) { 100, 100 }, 0);
            if (zox_tst_scroll_frames < zox_tst_scroll_hold_frames) {
                return 0;
            }
            if (!zox_tst_menus_click(world, player, main_menu, &button_event_menu_options, "options")) {
                return 1;
            }
            break;
        case 2: {
            zox_tst_waiting = "the options menu's layout";
            if (!options || zox_tst_scroll_frames < 10) {
                return 0;
            }
            zox_tst_scroll_view = zox_get_link(world, options, ScrollviewLink);
            zox_tst_scroll_list = zox_valid(zox_tst_scroll_view) ? zox_get_link(world, zox_tst_scroll_view, ListUILink) : 0;
            zox_tst_scroll_handle = zox_valid(zox_tst_scroll_view) ? get_scrollview_handle(world, zox_tst_scroll_view) : 0;
            zox_tst_check(zox_tst_scroll_view && zox_has(zox_tst_scroll_view, Scrollview), "options has no tagged scrollview");
            zox_tst_check(zox_tst_scroll_list, "options scrollview has no list");
            zox_tst_check(zox_tst_scroll_handle, "options scrollview has no scrollbar handle");
            if (!zox_tst_scroll_view || !zox_tst_scroll_list || !zox_tst_scroll_handle) {
                return 1;
            }
            zox_tst_scroll_max = get_scrollview_max_start(world, zox_tst_scroll_list);
            zox_tst_check(zox_tst_scroll_max >= 2, "options only scrolls [%i] elements, expected at least 2", zox_tst_scroll_max);
            // the first list element, the gap below it, and empty space off the window
            entity first_child = 0;
            iter it2 = zox_children(world, zox_tst_scroll_list);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count && !first_child; j++) {
                    if (zox_has(it2.entities[j], ChildIndex) && zox_getv(it2.entities[j], ChildIndex) == 0) {
                        first_child = it2.entities[j];
                    }
                }
            }
            CanvasViewport viewport;
            const byte found = first_child &&
                get_canvas_viewport(world, canvas, &viewport) &&
                zox_tst_ui_pointer_position(world, canvas, first_child, &zox_tst_scroll_on_list);
            zox_tst_check(found, "options list's first element has no screen position");
            if (!found || zox_tst_scroll_max < 2) {
                return 1;
            }
            const int4 bounds = get_element_pointer_bounds(world, &viewport, first_child,
                zox_getv(first_child, Position2), zox_getv(first_child, LayoutSize));
            // just under the first element: inside the scrollview, between list elements
            zox_tst_scroll_on_gap = (int2) { zox_tst_scroll_on_list.x, bounds.z - 1 };
            zox_tst_scroll_on_empty = (int2) {
                viewport.position.x + viewport.size.x - 4,
                viewport.position.y + viewport.size.y - 4
            };
            zox_tst_scroll_handle_top = zox_tst_scroll_handle_y(world);
            zox_log("[tst] options scrolls [%i] starts, list at [%ix%i], gap at [%ix%i], handle at [%i]",
                zox_tst_scroll_max, zox_tst_scroll_on_list.x, zox_tst_scroll_on_list.y,
                zox_tst_scroll_on_gap.x, zox_tst_scroll_on_gap.y, zox_tst_scroll_handle_top);
            zox_tst_check(zox_tst_scroll_start(world) == 0, "options starts scrolled to [%i]", zox_tst_scroll_start(world));
            break;
        }
        case 3:
            zox_tst_waiting = "a wheel notch down over the list";
            if (!zox_tst_scroll_notch(zox_tst_scroll_on_list, -1)) {
                return 0;
            }
            zox_tst_check(zox_tst_scroll_start(world) == 1, "one notch down: start is [%i], expected 1", zox_tst_scroll_start(world));
            zox_tst_check(zox_tst_scroll_handle_y(world) < zox_tst_scroll_handle_top,
                "one notch down: handle at [%i], expected below [%i]", zox_tst_scroll_handle_y(world), zox_tst_scroll_handle_top);
            break;
        case 4:
            zox_tst_waiting = "wheel notches past the end";
            if (!zox_tst_scroll_notch(zox_tst_scroll_on_list, -(zox_tst_scroll_max + 5))) {
                return 0;
            }
            zox_tst_check(zox_tst_scroll_start(world) == zox_tst_scroll_max, "past the end: start is [%i], expected [%i]", zox_tst_scroll_start(world), zox_tst_scroll_max);
            break;
        case 5:
            zox_tst_waiting = "wheel notches up past the top";
            if (!zox_tst_scroll_notch(zox_tst_scroll_on_list, zox_tst_scroll_max + 5)) {
                return 0;
            }
            {
                const entity target = zox_tst_mouse_target(world, player);
                zox_tst_check(zox_tst_scroll_start(world) == 0, "past the top (targeting [%s]): start is [%i], expected 0",
                    zox_valid(target) ? zox_getn(target) : "nothing", zox_tst_scroll_start(world));
            }
            zox_tst_check(zox_tst_scroll_handle_y(world) == zox_tst_scroll_handle_top,
                "past the top: handle at [%i], expected back at [%i]", zox_tst_scroll_handle_y(world), zox_tst_scroll_handle_top);
            break;
        case 6:
            zox_tst_waiting = "a wheel notch over empty space";
            if (!zox_tst_scroll_notch(zox_tst_scroll_on_empty, -1)) {
                return 0;
            }
            zox_tst_check(zox_tst_scroll_start(world) == 0, "wheel over empty space scrolled to [%i]", zox_tst_scroll_start(world));
            break;
        case 7: {
            zox_tst_waiting = "a wheel notch between list elements";
            if (!zox_tst_scroll_notch(zox_tst_scroll_on_gap, -1)) {
                return 0;
            }
            const entity target = zox_tst_mouse_target(world, player);
            zox_tst_check(zox_tst_scroll_start(world) == 1, "wheel between list elements (targeting [%s]): start is [%i], expected 1",
                zox_valid(target) ? zox_getn(target) : "nothing", zox_tst_scroll_start(world));
            break;
        }
        case 8: {
            // hovers the handle, then grabs it and drags it to the top
            zox_tst_waiting = "hovering the scrollbar handle";
            if (zox_tst_scroll_frames == 1) {
                // the part of the handle that's on screen (a small window can cut the options window off)
                CanvasViewport viewport;
                if (!get_canvas_viewport(world, canvas, &viewport)) {
                    return 0;
                }
                const int4 bounds = get_element_pointer_bounds(world, &viewport, zox_tst_scroll_handle,
                    zox_getv(zox_tst_scroll_handle, Position2), zox_getv(zox_tst_scroll_handle, LayoutSize));
                const int right = int_min(bounds.y, viewport.position.x + viewport.size.x - 1);
                zox_tst_check(bounds.x < right, "scrollbar handle is off screen, x [%i-%i]", bounds.x, bounds.y);
                if (bounds.x >= right) {
                    return 1;
                }
                zox_tst_scroll_on_list = (int2) { (bounds.x + right) / 2, (bounds.z + bounds.w) / 2 };
            }
            zox_tst_mouse(zox_tst_scroll_on_list, 0);
            if (zox_tst_scroll_frames < zox_tst_scroll_hold_frames) {
                return 0;
            }
            const entity target = zox_tst_mouse_target(world, player);
            zox_tst_check(target == zox_tst_scroll_handle, "hovering the handle at [%ix%i] targets [%s]",
                zox_tst_scroll_on_list.x, zox_tst_scroll_on_list.y, zox_valid(target) ? zox_getn(target) : "nothing");
            break;
        }
        case 9:
            zox_tst_waiting = "pressing the scrollbar handle";
            zox_tst_mouse(zox_tst_scroll_on_list, 1);
            if (zox_tst_scroll_frames < zox_tst_scroll_hold_frames) {
                return 0;
            }
            break;
        case 10:
            zox_tst_waiting = "dragging the scrollbar handle to the top";
            zox_tst_scroll_on_list.y += 8;
            zox_tst_mouse(zox_tst_scroll_on_list, 1);
            if (zox_tst_scroll_frames < 3 * zox_tst_scroll_hold_frames) {
                return 0;
            }
            break;
        default:
            zox_tst_waiting = "releasing the scrollbar handle";
            zox_tst_mouse(zox_tst_scroll_on_list, 0);
            if (zox_tst_scroll_frames < zox_tst_scroll_hold_frames) {
                return 0;
            }
            zox_tst_check(zox_tst_scroll_start(world) == 0, "dragging the handle to the top: start is [%i], expected 0 (handle at [%i], top [%i])",
                zox_tst_scroll_start(world), zox_tst_scroll_handle_y(world), zox_tst_scroll_handle_top);
            zox_tst_mouse_end();
            return 1;
    }
    zox_tst_scroll_stage++;
    zox_tst_scroll_frames = 0;
    return 0;
}

void zox_tst_scroll_wheel_check(ecs* world) {
    (void) world;
    zox_tst_mouse_end();
}
