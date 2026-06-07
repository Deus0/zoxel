#ifndef zox_particles
#define zox_particles

zoxc_entity(ParticlesEmitterLink);
zoxc_int(ParticleEmitRate);
#include "particles2/_.c"
#include "particles3/_.c"

zox_begin_module(Particles) {
    zoxd_entity(ParticlesEmitterLink);
    zoxd_int(ParticleEmitRate);
    zox_import_module(Particles2D);
    zox_import_module(Particles3D);
} zox_end_module(Particles);

#endif
