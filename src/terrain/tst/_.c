#include "tilemap.c"
#include "air_chunk.c"
#include "place_mesh.c"
#include "water.c"
#include "water_flow.c"
#include "water_splash.c"
#include "water_light.c"
#include "dig_light.c"
#include "block_frames.c"

#include "streamers.c"

#include "lod_chunk_lights.c"

#include "perf.c"
#include "lod_swap.c"
#include "render_fps.c"

void zox_tsts_terrain(ecs* world) {
    zox_tsts_tilemap(world);
    zox_tst_add("place_mesh", zox_tst_place_mesh_ready, zox_tst_place_mesh_check, 120);
    zox_tst_set_setup("place_mesh", NULL, 1);
    zox_tst_add("water_block", zox_tst_water_block_ready, zox_tst_water_block_check, 120);
    zox_tst_set_setup("water_block", NULL, 1);
    zox_tst_add("water_flow", zox_tst_water_flow_ready, zox_tst_water_flow_check, 120);
    zox_tst_set_setup("water_flow", NULL, 1);
    zox_tst_add("water_splash", water_splash_ready, water_splash_check, 60);
    zox_tst_set_setup("water_splash", NULL, 1);
    zox_tst_add("water_light", zox_tst_water_light_ready, zox_tst_water_light_check, 120);
    zox_tst_set_setup("water_light", NULL, 1);
    zox_tst_add("dig_light", zox_tst_dig_light_ready, zox_tst_dig_light_check, 120);
    zox_tst_add("block_frames", zox_tst_block_frames_ready, zox_tst_block_frames_check, 90);
    zox_tst_set_setup("block_frames", NULL, 1);
    zox_tst_add("water_tile", zox_tst_water_tile_ready, zox_tst_water_tile_check, 60);
    zox_tst_set_setup("water_tile", NULL, 1);
    zox_tsts_streamers(world);
    zox_tsts_lod_chunk_lights(world);
    zox_tsts_lod_swap(world);
    zox_tsts_perf(world);
    zox_tst_add("render_fps", zox_tst_render_fps_ready, zox_tst_render_fps_check, 600);
}
