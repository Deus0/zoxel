#include "collide.c"
// #include "remove.c"
#include "dots_remove.c"
#include "particles.c"
#include "sounds.c"

void zox_define_systems_auras(ecs* world) {
    zox_filter(
        characters,
        [in] combat.Dead,
        [in] transforms3.Position3D,
        [none] characters.Character
    );
    zox_system_ctx_1(
        AuraDotSystem,
        zoxp_mainthread,
        characters,
        [in] skills.SkillActive,
        [in] skills.SkillDamage,
        [in] skills.SkillRange,
        [in] colorz.Color,
        [none] auras.Aura
    );
    zox_system(
        AuraDotRemoveSystem,
        EcsOnUpdate,
        [in] skills.SkillLink,
        [in] skills.SpawnerLink,
        [in] particles.ParticlesEmitterLink,
        [none] skills.AuraDot
    );
    zox_system_1(
        AuraParticlesSystem,
        zoxp_mainthread,
        [in] timers.Activate,
        [in] skills.SkillActive,
        [in] skills.SkillRange,
        [in] colorz.Color,
        [out] particles.ParticlesEmitterLink,
        [none] auras.Aura
    );
    zox_system_1(
        AuraSoundSystem,
        zoxp_mainthread,
        [in] timers.Activate,
        [none] auras.Aura
    );
}
