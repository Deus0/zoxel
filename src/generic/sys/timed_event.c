extern double zox_delta_time;

void TimedEventSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TimedEvent);
    zox_sys_in(EventInput);
    zox_sys_out(EventTime);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TimedEvent, event);
        zox_sys_i(EventInput, input);
        zox_sys_o(EventTime, time);

        time->value -= zox_delta_time;
        if (time->value <= 0) {
            if (event->value) {
                (*event->value)(world, input->value);
            }
            zox_delete(e);
        }
    }
} zoxd_system2(TimedEventSystem);