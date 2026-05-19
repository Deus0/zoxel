#include "melee.c"
#include "damage_aura.c"
#include "damage_aura_remove.c"
#include "dots.c"
#include "activate.c"
#include "toggle.c"
#include "aura_particles.c"
#include "aura_sound.c"
#include "character.c"
realm_clear_system(SkillLinks);

void define_systems_skills(ecs *world) {
    realm_clear_systemd(skills, SkillLinks);
    zox_system(
        SkillToggleSystem,
        EcsOnUpdate,
        [in] timers.Activate,
        [out] skills.SkillActive,
        [none] Aura
    );
    // TODO: DotLinks just parent them instead
    zox_filter(
        characters,
        [in] combat.Dead,
        [in] transforms3.Position3D,
        [out] stats.DotLinks
    );
    zox_system_ctx_1(
        AuraSystem,
        zoxp_mainthread,
        characters,
        [in] SkillActive,
        [in] SkillDamage,
        [in] SkillRange,
        [in] colorz.Color,
        [none] Aura
    );
    zox_system(
        AuraRemoveSystem,
        EcsOnUpdate,
        [in] transforms3.Position3D,
        [out] stats.DotLinks
    );
    zox_system(
        DotsSystem,
        EcsOnUpdate,
        [in] skills.SpawnerLink,
        [in] skills.SkillDamage,
        [none] skills.Poison
    );
    // TODO: split into sound, resource and damage systems
    zox_system_1(
        MeleeSystem,
        zoxp_queue_add,
        [in] skills.SkillDamage,
        [in] skills.SkillDamageMax,
        [in] skills.SkillRange,
        [in] skills.SkillResourceLink,
        [in] skills.SkillCost,
        [in] timers.Activate,
        [none] skills.Melee
    );
    zox_system_1(
        CharacterSkillsSpawnSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [out] skills.SkillLinks
        // [none] !players.PlayerLink
    );
    // Spawn particles around player for Auras
    zox_system_1(
        AuraParticlesSystem,
        zoxp_mainthread,
        [in] timers.Activate,
        [in] skills.SkillActive,
        [in] skills.SkillRange,
        [in] colorz.Color,
        [out] particles.ParticlesEmitterLink,
        [none] skills.Aura
    );
    zox_system_1(
        AuraSoundSystem,
        zoxp_mainthread,
        [in] timers.Activate,
        [none] skills.Aura
    );
}
