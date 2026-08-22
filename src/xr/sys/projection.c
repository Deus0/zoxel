// NOTE: Increments Initialize, then removes it
void xr_projection_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Initialize);
    zox_sys_in(XrFov);
    zox_sys_out(ProjectionMatrix);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Initialize, state);
        zox_sys_i(XrFov, fov);
        zox_sys_o(ProjectionMatrix, projection);
        if (state->value != zox_dirty_active) {
            continue;
        }
        projection->value = xr_create_projection_matrix(
            fov,
            0.05f,
            100.0f
        );
    }
    zox_sys_on_end();
} zoxd_system(xr_projection_system);