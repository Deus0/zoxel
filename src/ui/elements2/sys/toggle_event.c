zox_sys2(ToggleEventSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ClickState);
    zox_sys_in(Clicker);
    zox_sys_in(ToggleEvent);
    zox_sys_out(ActiveState);
    zox_sys_out(ActiveStateDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ClickState, cstate);
        zox_sys_i(Clicker, clicker);
        zox_sys_i(ToggleEvent, event);
        zox_sys_o(ActiveState, tstate);
        zox_sys_o(ActiveStateDirty, dirty);

        if (cstate->value != zox_click_state_released_this_frame) {
            continue;
        }

        tstate->value = !tstate->value;
        dirty->value = zox_dirty_trigger;

        if (!event->value || !*event->value) {
            continue;
        }

        const ToggleEventData data = (ToggleEventData) {
            .e = e,
            .player = clicker->value,
            .value = tstate->value
        };
        (*event->value)(world, &data);

        // zox_log("Toggled the Toggle! I AM TOGGLER");
    }
} zox_sys_end(ToggleEventSystem);
