void generic_event_debug_system(iter* it) {
    zox_sys_on_begin();
    zox_log(" > events occurs [%i]\n", it->count);
    zox_sys_begin();
    zox_sys_in(GenericEvent);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenericEvent, genericEvent);
        zox_log("     > event occurs [%lu] [%i]", e, genericEvent->value);
    }
    zox_sys_on_end();
} zoxd_system(generic_event_debug_system);
