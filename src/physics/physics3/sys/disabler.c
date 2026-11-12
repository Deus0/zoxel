const byte physics_disable_frames = 8;

// holds a freshly spawned body still for its first physics_disable_frames frames
//  - runs after the forces are added and before the step, clearing them and its velocity
void physics3d_disable_system(iter* it) {
    zox_sys_on_begin();
#ifdef zoxel_disable_velocity
    return;
#endif
    zox_physics_profile_begin();
    zox_sys_begin();
    zox_sys_out(InitializePhysics3D);
    zox_sys_out(Velocity3D);
    zox_sys_out(Acceleration3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(InitializePhysics3D, initialize);
        zox_sys_o(Velocity3D, velocity);
        zox_sys_o(Acceleration3D, acceleration);
        if (initialize->value < physics_disable_frames) {
            initialize->value++;
            float3_make_zero(&velocity->value);
            float3_make_zero(&acceleration->value);
        }
    }
    zox_physics_profile_end();
    zox_sys_on_end();
} zoxd_system(physics3d_disable_system);
