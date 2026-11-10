// NOTE: Tells the arms to swing or not, from the skeletons owner velocity
zox_sys2(WalkStateSystem) {
    byte dbg_log = 0;
    float speed_cutoff = 0.3f;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(WalkState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(WalkState, state);
        entity owner = zox_get_parent(world, e);
        if (!zox_valid(owner) || !zox_has(owner, Velocity3D)) {
            continue;
        }
        float speed = float3_length(zox_getv(owner, Velocity3D));
        byte new_state = speed >= speed_cutoff;
        if (state->value != new_state) {
            state->value = new_state;
            if (dbg_log) {
                zox_log("Walk Speed [%f]", speed);
            }
        }
    }
} zox_sys_end(WalkStateSystem);
