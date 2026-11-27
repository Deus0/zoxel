#include "emit.c"
#include "stream.c"
#include "render.c"
#include "fade.c"

void zox_systems_particles3(ecs* world) {
    zox_observer(particle3_fade_on_life, EcsOnSet,
        [in] timing.DestroyInTime,
        [out] animations.FadeOutTime,
        [out] animations.AnimationStart,
        [none] Particle3D);
    zox_system_1(
        particle3_stream_system,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [in] transforms3.Bounds3D,
        [in] colorz.Color,
        [in] particles.ParticleEmitRate,
        [in] particles3.ParticleLife,
        [in] physics3.Gravity3D,
        [in] rendering.Brightness,
        [none] particles3.ParticleStreamer
    );
    zox_system_1(
        particle3d_emit_system,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [in] particles.ParticleEmitRate,
        [in] transforms3.Bounds3D,
        [in] colorz.Color,
        [in] rendering.Brightness,
        [none] Particle3DEmitter,
        [none] !particles3.ParticleStreamer
    );
    // if making rotation ones, just create new systems, add [none] transforms3.Rotation3D - for this one
    zox_render3_system(2,
        particle3d_render_system,
        [in] transforms3.Position3D,
        [in] colorz.Color,
        [in] rendering.Alpha,
        [in] transforms.Scale1,
        [in] rendering.Brightness,
        [none] Particle3D
    );
}
