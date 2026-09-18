/*
 *  Particles Module in 3D Space!
 *
 *      - spawns point like entities
 *      - renders them as points, lines, meshes etc
 *      - uses instancing
 *
 * */
#ifndef zoxm_particles3
#define zoxm_particles3

// #define zox_disable_particles_gpu_instancing

#include "dat/settings.c"
zox_tag(Particle3D);
zox_tag(Particle3DEmitter);

#include "sha/_.c"
#include "pre/_.c"
#include "fun/debug.c"
#include "fun/spawn.c"
#include "sys/_.c"
#include "dbg/_.c"

void update_statistics_particles3D() {
    zox_statistics_particles3D = 0;
}

zox_begin_module(Particles3D) {
    zoxd_tag(Particle3D);
    zoxd_tag(Particle3DEmitter);
    zox_systems_particles3(world);
    add_hook_load_shader(&spawn_shaders_particles3D);
    add_hook_spawn_prefabs(spawn_prefabs_particles3D);
} zox_end_module(Particles3D)

#endif
