// NOTE: Increments a timer with a state
void timer_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_in(TimerRate);
    zox_sys_out(TimerState);
    zox_sys_out(TimerTime);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TimerRate, rate);
        zox_sys_o(TimerState, state);
        zox_sys_o(TimerTime, timer);
        state->value = 0;
        if (timer->value == 0) {
            timer->value = rate->value;
        } else {
            timer->value -= zox_delta_time;
            if (timer->value <= 0) {
                timer->value = 0;
                state->value = 1;
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(timer_system);