#include "collide.c"
// #include "remove.c"
#include "dots_remove.c"
#include "particles.c"
#include "sounds.c"

void zox_define_systems_auras(ecs* world) {
    zox_filter(
        characters,
        [in] transforms3.Position3D,
        [none] characters.Character,
        [none] !core.Dead,
    );
    zox_system_ctx_1(
        AuraDotSystem,
        zoxp_spawn,
        characters,
        [in] skills.SkillActive,
        [in] skills.SkillDamage,
        [in] skills.SkillRange,
        [in] colorz.Color,
        [none] auras.Aura
    );
    zox_system(
        AuraDotRemoveSystem,
        zoxp_update,
        [in] skills.SkillLink,
        [in] skills.SpawnerLink,
        [in] particles.ParticlesEmitterLink,
        [none] skills.AuraDot
    );
    zox_system_1(
        AuraParticlesSystem,
        zoxp_spawn,
        [in] timers.Activate,
        [in] skills.SkillActive,
        [in] skills.SkillRange,
        [in] colorz.Color,
        [out] particles.ParticlesEmitterLink,
        [none] auras.Aura
    );
    zox_system_1(
        AuraSoundSystem,
        zoxp_spawn,
        [in] timers.Activate,
        [none] auras.Aura
    );
}
