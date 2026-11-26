// The popup and options survive opening, picking and clicking outside.
static byte zox_tst_dropdown_ready(ecs* world) {
    static int step;
    static entity dropdown, popup, option;
    zox_tst_progress(step);
    switch (step) {
        case 0: {
            const entity canvas = zox_tst_ui_fixture_canvas(world);
            if (!canvas) {
                return 0;
            }
            const char* options[] = { "name", "tag", "type" };
            dropdown = spawn_dropdown(world, canvas, canvas, int2_zero, (int2) { 120, 24 },
                float2_half, options, 3, 0, 12, 32);
            popup = item_list_popup(world, dropdown);
            option = item_list_item_at(world, dropdown, 1);
            zox_tst_check(popup && option && item_list_item_at(world, dropdown, 2), "missing popup or option entities");
            zox_tst_check(!item_list_is_open(world, dropdown), "popup visible at spawn");
            zox_tst_check(zox_get_link(world, dropdown, DropdownSelectedLink) == item_list_item_at(world, dropdown, 0),
                "first item wasn't selected at spawn");
            break;
        }
        case 1:
            zox_tst_click(world, dbg_player, dropdown);
            break;
        case 2:
            zox_tst_check(item_list_is_open(world, dropdown), "popup didn't open");
            zox_tst_check(item_list_item_at(world, dropdown, 1) == option && !zox_getv(option, RenderDisabled),
                "opening didn't show the existing option");
            zox_tst_click(world, dbg_player, option);
            break;
        case 3: {
            const entity label = zox_get_child_by_id(world, dropdown, zox_id(TextLabel));
            zox_tst_check(!item_list_is_open(world, dropdown), "picking didn't close the popup");
            zox_tst_check(zox_get_link(world, dropdown, DropdownSelectedLink) == option, "wrong selected item");
            zox_tst_check(is_zext(zox_getm(label, TextData), "tag"), "button label didn't show the selected item");
            zox_tst_click(world, dbg_player, dropdown);
            break;
        }
        case 4:
            zox_tst_check(item_list_is_open(world, dropdown), "popup didn't reopen");
            zox_tst_mouse((int2) { 4, 4 }, 0);
            break;
        case 5:
            zox_tst_mouse((int2) { 4, 4 }, 1);
            break;
        case 6:
            zox_tst_mouse((int2) { 4, 4 }, 0);
            break;
        case 7:
            zox_tst_check(!item_list_is_open(world, dropdown), "outside click didn't close the popup");
            zox_tst_check(item_list_popup(world, dropdown) == popup && item_list_item_at(world, dropdown, 1) == option,
                "popup or item was replaced");
            zox_tst_check(zox_get_link(world, dropdown, DropdownSelectedLink) == option, "outside click changed selection");
            return 1;
    }
    step++;
    return 0;
}
