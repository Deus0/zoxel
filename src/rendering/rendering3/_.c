byte transparent_meshes = 0;
byte paint_weights = 0;     // skeleton meshes coloured by their bones (bone.vert), the Paint Weights setting, tst body_weights
byte zox_grass_animation = 1;           // setting: "Grass Animation"
float zox_grass_sway_strength = 0.07f;  // world units the grass tips move
// draw counters, only ever increase, read by tests (rendering3/tst)
uint zox_drawn_skeleton_meshes = 0;
uint zox_drawn_vox_instances = 0;
uint zox_drawn_chunks = 0;
double zox_vox_instance_render_ms = 0;    // vox_instance_render_system's time, summed (tst vox_instances_stress)

#include "dat/_.c"
#include "mat/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "eve/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_rendering3(ecs* world) {
    zox_module(rendering3);
    zox_components_rendering3(world);
    zox_systems_basics3(world);
#ifdef zox_tests
    zox_tsts_rendering3(world);
#endif
    add_hook_load_shader(&spawn_shaders_basics3D);
    add_hook_spawn_prefabs(&zox_prefabs_rendering3);
    add_hook_spawn_prefabs(&zox_events_rendering3);
}

