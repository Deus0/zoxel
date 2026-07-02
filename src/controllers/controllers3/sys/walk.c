// NOTE: Tells the arms to swing or not
zox_sys2(WalkStateSystem) {
    byte dbg_log = 0;
    float speed_cutoff = 0.3f;
    zox_sys_begin();
    zox_sys_in(Velocity3D);
    zox_sys_out(WalkState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Velocity3D, velocity);
        zox_sys_o(WalkState, state);
        float speed = float3_length(velocity->value);
        byte new_state = speed >= speed_cutoff;
        if (state->value != new_state) {
            state->value = new_state;
            if (dbg_log) {
                zox_log("Walk Speed [%f]", speed);
            }
        }
    }
} zox_sys_end(WalkStateSystem);
