#ifndef zoxm_particles3
#define zoxm_particles3

#include "dat/settings.c"
zox_tag(Particle3D);
zox_tag(Particle3DEmitter);
#include "sha/particle_shader.c"
#include "pre/_.c"
#include "fun/debug.c"
#include "fun/spawn.c"
#include "sys/_.c"
#include "dbg/_.c"

void update_statistics_particles3D() {
    zox_statistics_particles3D = 0;
}

zox_begin_module(Particles3D)
    add_hook_load_shader(&spawn_shaders_particles3D);
    add_hook_spawn_prefabs(spawn_prefabs_particles3D);
    zoxd_tag(Particle3D);
    zoxd_tag(Particle3DEmitter);
    zox_system_1(
        Particle3DEmitSystem,
        EcsPreStore,
        [in] transforms3.Position3D,
        [in] particles.ParticleEmitRate,
        [in] generic.Bounds3D,
        [in] colorz.Color,
        [none] Particle3DEmitter
    );
    // if making rotation ones, just create new systems, add [none] transforms3.Rotation3D - for this one
    zox_render3D_plus_system(
        Particle3DRenderSystem,
        [in] transforms3.Position3D,
        [in] colorz.Color,
        [none] Particle3D
    );
zox_end_module(Particles3D)

#endif
