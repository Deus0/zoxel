// tests: the simulator's (sim.c); the shared maths are tested in cameras (cameras/tst/xr.c)
//  - tst/cube.c, math.c, opengl.c, test_cubes.c are the standalone app's (bsh/android.sh --testxr), not these
#include "sim.c"

void zox_tsts_xr(ecs* world) {
    zox_tsts_xr_sim(world);
}
