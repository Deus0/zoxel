#include "dots.c"
#include "activate.c"
#include "toggle.c"
#include "character.c"
#include "melee.c"
#include "shoot.c"
#include "pounce.c"
#include "tentacle.c"
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
        tentacle_activate_system,
        zoxp_spawn,
        [in] timers.Activate,
        [in] skills.SkillRange,
        [none] skills.Tentacle
    );
    zox_system_1(
        tentacle_system,
        zoxp_spawn,
        [in] skills.SkillUsing,
        [in] skills.SkillUsedAt,
        [in] skills.SkillTarget,
        [in] transforms3.Position3D
    );
    zox_system_1(
        pounce_system,
        zoxp_spawn,
        [in] skills.SkillUsing,
        [in] skills.SkillUsedAt,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [in] physics.Grounded,
        [in] vrays.RaycastVoxelData,
        [out] transforms3.Scale3,
        [out] physics3.Velocity3D
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
