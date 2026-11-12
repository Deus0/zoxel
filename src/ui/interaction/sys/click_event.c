// NOTE: Runs the ClickEvent on Click, added on release (or on press for ClickOnPress), see on_element_pressed
void ButtonClickEventSystem(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ClickEvent);
    zox_sys_out(Clicker);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ClickEvent, event);
        zox_sys_o(Clicker, clicker);
        if (!event->value || !*event->value) {
            continue;
        }
        ClickEventData event_data = (ClickEventData) {
            .clicked = e,
            .clicker = clicker->value
        };
        (*event->value)(world, event_data);
    }
    zox_sys_on_end();
} zoxd_system(ButtonClickEventSystem);
