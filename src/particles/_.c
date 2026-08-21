/*
 * +------------------------------------------------------------------+
 * | Zox Module: Particles                                            |
 * |                                                                  |
 * |  Emitters - Effects - Motion - Lifetimes - Particle State        |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
zox_tag(Particle);
zoxc_entity(ParticlesEmitterLink);
zoxc_int(ParticleEmitRate);
#include "particles2/_.c"
#include "particles3/_.c"

void import_particles(ecs* world) {
    zox_module(particles);
    zoxd_tag(Particle);
    zoxd_entity(ParticlesEmitterLink);
    zoxd_int(ParticleEmitRate);
    zox_import_module(Particles2D);
    zox_import_module(Particles3D);
}
