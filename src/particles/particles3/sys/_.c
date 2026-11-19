#include "emit.c"
#include "stream.c"
#include "render.c"

void zox_systems_particles3(ecs* world) {
    zox_system_1(
        particle3_stream_system,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [in] transforms3.Bounds3D,
        [in] colorz.Color,
        [in] particles3.ParticlesPerSecond,
        [in] particles3.ParticleLife,
        [in] particles3.ParticleRise,
        [out] particles3.ParticlesDue,
        [none] particles3.ParticleStreamer
    );
    zox_system_1(
        particle3d_emit_system,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [in] particles.ParticleEmitRate,
        [in] transforms3.Bounds3D,
        [in] colorz.Color,
        [none] Particle3DEmitter,
        [none] !particles3.ParticleStreamer
    );
    // if making rotation ones, just create new systems, add [none] transforms3.Rotation3D - for this one
    zox_render3_system(2,
        particle3d_render_system,
        [in] transforms3.Position3D,
        [in] colorz.Color,
        [none] Particle3D
    );
}
