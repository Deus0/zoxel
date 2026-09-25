extern double zox_delta_time;

zox_sys2(TimedEventSystem) {
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
                entity e1 = input->value;
                if (zox_has(e, EventInput2)) {
                    entity e2 = zox_getv(e, EventInput2);
                    ((void (*)(ecs*, entity, entity)) event->value)(world, e1, e2);
                } else {
                    ((void (*)(ecs*, entity)) event->value)(world, e1);
                }
                // (*event->value)(world, input->value);
            }
            zox_delete(e);
        }
    }
} zox_sys_end(TimedEventSystem);
