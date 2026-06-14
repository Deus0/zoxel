// NOTE: Tell the light systems to update when its all done
zox_sys2(Chunk3GeneratedSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_out(GenerateLights);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, state);
        zox_sys_o(GenerateLights, dirty);
        if (state->value == zox_dirty_end) {
            dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(Chunk3GeneratedSystem);
