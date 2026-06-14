const byte physics_disable_frames = 8;

zox_sys2(Physics3DDisableSystem) {
#ifdef zoxel_disable_velocity
    return;
#endif
    zox_sys_begin();
    zox_sys_out(InitializePhysics3D);
    zox_sys_out(Position3D);
    zox_sys_out(Velocity3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(InitializePhysics3D, initialize);
        zox_sys_o(Position3D, position);
        zox_sys_o(Velocity3D, velocity);
        if (initialize->value < physics_disable_frames) {
            initialize->value++;
            position->value.x -= velocity->value.x * zox_delta_time;
            position->value.y -= velocity->value.y * zox_delta_time;
            position->value.z -= velocity->value.z * zox_delta_time;
            float3_make_zero(&velocity->value);
        }
    }
} zox_sys_end(Physics3DDisableSystem);
