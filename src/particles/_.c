/*
 * +------------------------------------------------------------------+
 * | Zox Module: Particles                                            |
 * |                                                                  |
 * |  Emitters - Effects - Motion - Lifetimes - Particle State        |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
zox_tag(Particle);
zox_tag(ParticlesEmitter);
zoxc_int(ParticleEmitRate);
#include "particles2/_.c"
#include "particles3/_.c"

void import_particles(ecs* world) {
    zox_module(particles);
    zoxd_tag(Particle);
    zoxd_tag(ParticlesEmitter);
    zoxd_int(ParticleEmitRate);
    zox_import_module(Particles2D);
    zox_import_module(Particles3D);
}
