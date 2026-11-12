#include "dots.c"
#include "activate.c"
#include "toggle.c"
#include "character.c"
#include "melee.c"
#include "shoot.c"
#include "projectile_hit.c"

void zox_systems_skills(ecs *world) {
    zox_system(
        skill_toggle_system,
        zoxp_update,
        [in] timers.Activate,
        [out] skills.SkillActive,
        [none] skills.SkillToggleable
    );
    zox_system(
        dots_system,
        zoxp_update,
        [in] skills.SkillDamage,
        [none] skills.Dot
    );
    // TODO: split into sound, resource and damage systems
    zox_system_1(
        melee_system,
        zoxp_spawn,
        [in] skills.SkillCost,
        [in] skills.SkillDamage,
        [in] skills.SkillDamageMax,
        [in] skills.SkillRange,
        [in] timers.Activate,
        [none] skills.Melee
    );
    // TODO: Make skill activate use resource
    zox_system_1(
        shoot_system,
        zoxp_spawn,
        [in] timers.Activate,
        [in] skills.SkillRange,
        [in] skills.SkillDamage,
        [in] skills.SkillDamageMax,
        [none] skills.Shoot
    );
    zox_system_1(
        projectile_hit_system,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [in] physics3.Velocity3D,
        [in] combat.Damage,
        [none] projectiles.Projectile
    );
}
