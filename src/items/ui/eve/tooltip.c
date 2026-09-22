// Tooltip Event for item icons
void item_icon_tooltip_event(iter* it) {
    // byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DataLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DataLink, datac);
        /*if (state->value != zox_state_select_active) {
            continue;
        }*/
        // zox_log("icon_tooltip_event [%s]", zox_sys_e_name);
        entity data = datac->value;
        // TODO: We should do this for only X types of items
        //      - BlockItem, ConsumeableItem, etc
        if (!zox_valid(data) ||
            !zox_has(data, Item) ||
            !zox_has(data, Quantity))
        {
            continue;
        }
        entity canvas = zox_get_parent_by_id(
            world,
            e,
            zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        entity tooltip = zox_get_child_by_id(
            world,
            canvas,
            zox_id(Tooltip));
        if (!tooltip) {
            zox_loge("Tooltip not found in canvas");
            continue;
        }
        const char* name =
            zox_has(data, ZoxName) ?
                zox_getv(data, ZoxName) :
                zox_getn(data);
        byte quantity =
            zox_has(data, Quantity) ?
                zox_getv(data, Quantity) :
                1;
        char result[128];
        sprintf(
            result,
            "[%s] x%i\n",
            name,
            quantity);
        set_tooltip_text(
            world,
            e,
            tooltip,
            result);
    }
}
