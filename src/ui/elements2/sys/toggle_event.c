// an observer (the ui event: a non fragmenting tag, its cleanup observer removes it)
void toggle_event_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Clicker);
    zox_sys_in(ToggleEvent);
    zox_sys_out(ActiveState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Clicker, clicker);
        zox_sys_i(ToggleEvent, event);
        zox_sys_o(ActiveState, tstate);
        tstate->value = !tstate->value;
        zox_add(e, ActiveStateDirty);
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
}
