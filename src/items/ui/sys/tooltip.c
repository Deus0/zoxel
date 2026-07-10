// Tooltip Event for item icons
zox_sys2(ItemIconTooltipSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SelectState);
    zox_sys_in(DataLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SelectState, state);
        zox_sys_i(DataLink, data);
        if (state->value != zox_state_select_active) {
            continue;
        }
        entity e2 = data->value;
        if (!zox_valid(e2) || !zox_has(e2, Item) || !zox_has(e2, Quantity)) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
        if (!tooltip) {
            zox_loge("Tooltip not found in canvas");
            continue;
        }
        const char* name = zox_has(e2, ZoxName) ? zox_getv(e2, ZoxName) : zox_get_name(e2);
        byte quantity = zox_has(e2, Quantity) ? zox_getv(e2, Quantity) : 1;
        char result[128];
        sprintf(result, "[%s] x%i\n", name, quantity);
        set_entity_text(world, tooltip, result);
    }
} zox_sys_end(ItemIconTooltipSystem);
