// terrain collision tests (terrain_collision_system), each on a tiny terrain of its own, gravity any way (frame.c)
//  - terrain_collisions: landing, fast falls, walls, seams, edges, steps, holes (collisions.c)
//  - terrain_ledges: a body as big as a character gets into gaps 2 blocks tall, up a step and off their middle (ledges.c)
//  - run: bash bsh/tst.sh terrain_collisions (or _10fps, _30fps, _up, _x, _z), terrain_ledges (or _10fps, _30fps, _x)
#include "frame.c"
#include "collisions.c"
#include "ledges.c"

void zox_tsts_terrain_collisions() {
    zox_tst_add_physics("terrain_collisions", zox_tst_terrain_collisions_ready, zox_tst_terrain_collisions_check, 40);
    zox_tst_add("terrain_collisions_up", zox_tst_terrain_collisions_ready, zox_tst_terrain_collisions_check, 40);
    zox_tst_set_setup("terrain_collisions_up", zox_tst_tc_frame_up_setup, 0);
    zox_tst_add("terrain_collisions_x", zox_tst_terrain_collisions_ready, zox_tst_terrain_collisions_check, 40);
    zox_tst_set_setup("terrain_collisions_x", zox_tst_tc_frame_x_setup, 0);
    zox_tst_add("terrain_collisions_z", zox_tst_terrain_collisions_ready, zox_tst_terrain_collisions_check, 40);
    zox_tst_set_setup("terrain_collisions_z", zox_tst_tc_frame_z_setup, 0);
    zox_tst_add_physics("terrain_ledges", zox_tst_terrain_ledges_ready, zox_tst_terrain_ledges_check, 40);
    zox_tst_add("terrain_ledges_x", zox_tst_terrain_ledges_ready, zox_tst_terrain_ledges_check, 40);
    zox_tst_set_setup("terrain_ledges_x", zox_tst_tc_frame_x_setup, 0);
}
