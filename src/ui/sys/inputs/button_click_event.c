zox_sys2(ButtonClickEventSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ClickEvent);
    zox_sys_in(ClickState);
    zox_sys_out(Clicker);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ClickEvent, event);
        zox_sys_i(ClickState, state);
        zox_sys_o(Clicker, clicker);

        if (state->value != zox_click_state_released_this_frame) {
            continue;
        }

        if (!event->value || !*event->value) {
            continue;
        }

        const ClickEventData event_data = (ClickEventData) {
            .clicked = e,
            .clicker = clicker->value
        };
        (*event->value)(world, event_data);
    }
} zox_sys_end(ButtonClickEventSystem);
