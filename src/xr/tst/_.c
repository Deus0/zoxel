// tests: room scale (room.c), the simulator's (sim.c), its controllers on the 3D menus and into a game (menu.c); the shared maths are tested in cameras (cameras/tst/xr.c)
//  - tst/cube.c, math.c, opengl.c, test_cubes.c are the standalone app's (bsh/android.sh --testxr), not these
#include "sim.c"
#include "menu.c"
#include "room.c"
#include "body.c"
#include "frustum.c"
#ifdef zox_tests
#include "continue.c"     // load_games' saves (huds tests, zox_tests only)
#endif

void zox_tsts_xr(ecs* world) {
    zox_tsts_xr_sim(world);
    zox_tsts_xr_menu(world);
    zox_tsts_xr_play(world);
    zox_tsts_xr_room(world);
    zox_tsts_xr_body(world);
    zox_tsts_xr_frustum(world);
#ifdef zox_tests
    zox_tsts_xr_continue(world);
#endif
}
