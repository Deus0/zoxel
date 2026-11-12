// Physics tests, accuracy and performance of the 2D and 3D physics systems
//  - every test spawns its own bodies (no terrain, no game) and runs at a fixed dt
//  - accuracy: physics2_motion, physics2_rotation, physics3_motion, physics3_rotation
//      each also as _10fps and _30fps
//  - the exact step on its own: physics_steps
//  - performance: physics2_perf, physics3_perf
//  - terrain collisions are tested in src/terrain/collisions/tst
//  - run: bash bsh/tst.sh <name>
#include "common.c"
#include "physics2.c"
#include "physics3.c"
#include "perf.c"
#include "steps.c"

// adds an accuracy test at 60fps, and at 10fps (the biggest dt the engine lets through,
// max_zox_delta_time) and 30fps, its tolerances scale with dt
//  - zox_tst_physics_120fps works too, left out as small steps are the easy case and slow here
#define zox_tst_add_physics(name, ready, check, timeout) {\
    zox_tst_add(name, ready, check, timeout);\
    zox_tst_add(name "_10fps", ready, check, timeout);\
    zox_tst_set_setup(name "_10fps", zox_tst_physics_10fps, 0);\
    zox_tst_add(name "_30fps", ready, check, timeout);\
    zox_tst_set_setup(name "_30fps", zox_tst_physics_30fps, 0);\
}

void zox_tsts_physics() {
    zox_tst_add("physics_steps", zox_tst_physics_steps_ready, zox_tst_physics_steps_check, 30);
    zox_tst_add_physics("physics2_motion", zox_tst_physics2_motion_ready, zox_tst_physics2_motion_check, 30);
    zox_tst_add_physics("physics2_rotation", zox_tst_physics2_rotation_ready, zox_tst_physics2_rotation_check, 30);
    zox_tst_add_physics("physics3_motion", zox_tst_physics3_motion_ready, zox_tst_physics3_motion_check, 30);
    zox_tst_add_physics("physics3_rotation", zox_tst_physics3_rotation_ready, zox_tst_physics3_rotation_check, 30);
    zox_tst_add("physics2_perf", zox_tst_physics2_perf_ready, zox_tst_physics2_perf_check, 60);
    zox_tst_add("physics3_perf", zox_tst_physics3_perf_ready, zox_tst_physics3_perf_check, 60);
}
