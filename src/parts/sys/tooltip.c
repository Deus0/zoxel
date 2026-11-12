// NOTE: Tooltips for Body/Equip Items
void SlotItemTooltipSystem(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SelectState);
    // zox_sys_in(SlotLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SelectState, state);
        entity data = zox_get_link(world, e, DataLink);
        // zox_sys_i(SlotLink, slot);
        if (state->value != zox_state_select_active) {
            continue;
        }
        entity slot = zox_get_link(world, e, SlotLink);
        entity e2 = data;
        if (!zox_valid(e2) || !zox_has(e2, SlotType)) {
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
        entity body = zox_get_parent_by_id(
            world,
            slot,
            zox_id(Body));
        const char* name = zox_has(e2, ZoxName) ?
            zox_getv(e2, ZoxName) :
            zox_get_name(e2);
        byte slot_type = zox_getv(e2, SlotType);
        const char* slot_name =
            slot_type < slot_names_length ?
                slot_names[slot_type] :
                "unknown";
        char result[128];
        sprintf(result, "[%s]\n- Slot [%s]%s\n",
            name,
            slot_name,
            zox_valid(body) ?
                "+" :
                "");
        set_tooltip_text(
            world,
            e,
            tooltip,
            result);
    }
    zox_sys_on_end();
} zoxd_system(SlotItemTooltipSystem);
