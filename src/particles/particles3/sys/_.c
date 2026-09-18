#include "emit.c"
#include "render.c"

void zox_systems_particles3(ecs* world) {
    zox_system_1(
        Particle3DEmitSystem,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [in] particles.ParticleEmitRate,
        [in] transforms3.Bounds3D,
        [in] colorz.Color,
        [none] Particle3DEmitter
    );
    // if making rotation ones, just create new systems, add [none] transforms3.Rotation3D - for this one
    zox_render3_system(2,
        Particle3DRenderSystem,
        [in] transforms3.Position3D,
        [in] colorz.Color,
        [none] Particle3D
    );
}
