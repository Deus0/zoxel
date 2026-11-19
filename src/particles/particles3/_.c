/*
 *  Particles Module in 3D Space!
 *
 *      - spawns point like entities
 *      - renders them as points, lines, meshes etc
 *      - uses instancing
 *
 * */
// #define zox_disable_particles_gpu_instancing

#include "dat/settings.c"
zox_tag(Particle3D);
zox_tag(Particle3DEmitter);
zoxc_float(ParticlesPerSecond);  // a stream of particles (sys/stream.c)
zoxc_float(ParticlesDue);        // its part of a particle carried to the next frame
zoxc_float(ParticleLife);        // seconds each lives
zoxc_float3(ParticleSpread);     // half extents they start in
zoxc_float(ParticleRise);        // their speed up

#include "sha/_.c"
#include "pre/_.c"
#include "fun/debug.c"
#include "fun/spawn.c"
#include "fun/burst.c"
#include "sys/_.c"
#include "dbg/_.c"

void update_statistics_particles3D() {
    zox_statistics_particles3D = 0;
}

void import_particles3(ecs* world) {
    zox_module(particles3);
    zoxd_tag(Particle3D);
    zoxd_tag(Particle3DEmitter);
    zoxd_float(ParticlesPerSecond);
    zoxd_float(ParticlesDue);
    zoxd_float(ParticleLife);
    zoxd_float3(ParticleSpread);
    zoxd_float(ParticleRise);
    zoxd(MaterialParticle3D);
    zox_systems_particles3(world);
    add_hook_load_shader(&spawn_shaders_particles3D);
    add_hook_gpu_dispose(gpu_dispose_particles3);
    add_hook_gpu_restore(gpu_restore_particles3);
    add_hook_spawn_prefabs(spawn_prefabs_particles3D);
}
