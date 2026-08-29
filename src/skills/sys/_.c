#include "dots.c"
#include "activate.c"
#include "toggle.c"
#include "character.c"
#include "melee.c"
#include "shoot.c"

void zox_systems_skills(ecs *world) {
    zox_system(
        SkillToggleSystem,
        zoxp_update,
        [in] timers.Activate,
        [out] skills.SkillActive,
        [none] skills.SkillToggleable
    );
    zox_system(
        DotsSystem,
        zoxp_update,
        [in] skills.SpawnerLink,
        [in] skills.SkillDamage,
        [none] skills.Dot
    );
    // TODO: split into sound, resource and damage systems
    zox_system_1(
        MeleeSystem,
        zoxp_spawn,
        [in] skills.SkillResourceLink,
        [in] skills.SkillCost,
        [in] skills.SkillDamage,
        [in] skills.SkillDamageMax,
        [in] skills.SkillRange,
        [in] timers.Activate,
        [none] skills.Melee
    );
    // TODO: Make skill activate use resource
    zox_system_1(
        ShootSystem,
        zoxp_spawn,
        [in] timers.Activate,
        [in] skills.SkillRange,
        [in] skills.SkillDamage,
        [in] skills.SkillDamageMax,
        [none] skills.Shoot
    );
    /*zox_system_1(
        CharacterSkillsSpawnSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [out] skills.SkillLinks
        // [none] !players.PlayerLink
    );*/
}
