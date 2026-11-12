#include "collide.c"
// #include "remove.c"
#include "dots_remove.c"
#include "particles.c"
#include "sounds.c"

void zox_systems_auras(ecs* world) {
    zox_filter(
        characters,
        [in] transforms3.Position3D,
        [none] characters.Character,
        [none] !core.Dead,
    );
    zox_system_ctx_1(
        aura_dot_system,
        zoxp_spawn,
        characters,
        [in] skills.SkillActive,
        [in] skills.SkillDamage,
        [in] skills.SkillRange,
        [in] colorz.Color,
        [none] auras.Aura
    );
    zox_system(
        aura_dot_remove_system,
        zoxp_update,
        [none] skills.AuraDot
    );
    zox_system_1(
        aura_particles_system,
        zoxp_spawn,
        [in] timers.Activate,
        [in] skills.SkillActive,
        [in] skills.SkillRange,
        [in] colorz.Color,
        [none] auras.Aura
    );
    zox_system_1(
        aura_sound_system,
        zoxp_spawn,
        [in] timers.Activate,
        [none] auras.Aura
    );
}
