// tests: room scale (room.c), the simulator's (sim.c), its controllers on the 3D menus and into a game (menu.c); the shared maths are tested in cameras (cameras/tst/xr.c)
//  - tst/cube.c, math.c, opengl.c, test_cubes.c are the standalone app's (bsh/android.sh --testxr), not these
#include "sim.c"
#include "menu.c"
#include "room.c"
#include "body.c"
#include "lean.c"
#include "frustum.c"
#include "reach.c"
#include "options.c"
#include "grip.c"
#ifdef zox_tests
#include "continue.c"     // load_games' saves (huds tests, zox_tests only)
#endif

void zox_tsts_xr(ecs* world) {
    zox_tsts_xr_sim(world);
    zox_tsts_xr_menu(world);
    zox_tsts_xr_play(world);
    zox_tsts_xr_room(world);
    zox_tsts_xr_body(world);
    zox_tsts_xr_lean(world);
    zox_tsts_xr_frustum(world);
    zox_tsts_xr_reach(world);
    zox_tsts_xr_options(world);
    zox_tsts_xr_grip(world);
#ifdef zox_tests
    zox_tsts_xr_continue(world);
#endif
}
