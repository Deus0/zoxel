// tst: window_search, a window's search button shows its search bar, the body fits under it, closing clears it
//  (ins/search.c, sys/search.c)
static int zox_tst_window_body_height(ecs* world, entity window) {
    const entity body = zox_get_child_by_id(world, window, zox_id(WindowBody));
    return body ? zox_getv(body, LayoutSize).y : 0;
}

static byte zox_tst_window_search_ready(ecs* world) {
    static int step, body_height;
    static entity canvas, window, button, bar, dropdown;
    char text[32];
    byte type = 255;
    zox_tst_progress(step);
    switch (step) {
        case 0:
            zox_tst_waiting = "the fixture canvas";
            canvas = zox_tst_ui_fixture_canvas(world);
            if (!canvas) {
                return 0;
            }
            window = spawn_window_list_at(world, prefab_window, canvas, int2_zero, (int2) { 220, 160 },
                zox_alignment_left, float2_half, "Search", 18, 12, byte2_single(2), 6, 1,
                (ClickEvent) { button_event_close_window }).x;
            button = spawn_window_search_button(world, window, zox_get_child_by_id(world, window, zox_id(Header)));
            step++;
            return 0;
        case 1:
            zox_tst_waiting = "the window's body";
            body_height = zox_tst_window_body_height(world, window);
            if (!body_height) {
                return 0;
            }
            zox_tst_check(button && zox_has(button, WindowSearchButton), "no search button in the header");
            zox_tst_check(!window_search_get(world, window, text, sizeof(text), &type), "a search before opening");
            zox_tst_click(world, dbg_player, button);
            step++;
            return 0;
        case 2:
            zox_tst_waiting = "the search bar";
            bar = window_search_bar_shown(world, window);
            if (!bar) {
                return 0;
            }
            zox_tst_check(zox_tst_window_body_height(world, window) == body_height - zox_getv(bar, LayoutSize).y,
                "body is [%i] high with the bar open, not [%i]", zox_tst_window_body_height(world, window),
                body_height - zox_getv(bar, LayoutSize).y);
            input_field_set_value(world, zox_get_child_by_id(world, bar, zox_id(TextInput)), "Abc");
            dropdown = zox_get_child_by_id(world, bar, zox_id(Dropdown));
            dropdown_set_selected(world, dropdown, item_list_item_at(world, dropdown, window_search_tag));
            step++;
            return 0;
        case 3:
            zox_tst_check(window_search_get(world, window, text, sizeof(text), &type), "no search with the bar open");
            zox_tst_check(!strcmp(text, "abc"), "search text is [%s], not [abc]", text);
            zox_tst_check(type == window_search_tag, "search type is [%i], not tag", type);
            zox_tst_check(window_search_match("Big ABCs", text) && !window_search_match("xyz", text),
                "window_search_match is wrong");
            zox_tst_click(world, dbg_player, button);
            step++;
            return 0;
        case 4:
            zox_tst_waiting = "the search bar to close";
            if (window_search_bar_shown(world, window)) {
                return 0;
            }
            zox_tst_check(!window_search_get(world, window, text, sizeof(text), &type), "a search after closing");
            zox_tst_check(zox_tst_window_body_height(world, window) == body_height, "body is [%i] high, not back to [%i]",
                zox_tst_window_body_height(world, window), body_height);
            zox_tst_check(zox_get_link(world, dropdown, DropdownSelectedLink) ==
                item_list_item_at(world, dropdown, window_search_name), "closing didn't reset the filter type");
            return 1;
    }
    return 0;
}
