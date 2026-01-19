#include "melee.c"
#include "damage_aura.c"
#include "damage_aura_remove.c"
#include "dots.c"
#include "activate.c"
#include "toggle.c"
#include "aura_sound.c"
#include "character.c"
#include "realm.c"
realm_clear_system(SkillLinks);

void define_systems_skills(ecs *world) {

    realm_clear_systemd(skills, SkillLinks);

    zox_system_1(
        SkillsRealmSpawnSystem,
        EcsOnLoad,
        [in] realms.GenerateRealm,
        [in] stats.StatLinks,
        [out] skills.SkillLinks,
        [none] realms.Realm
    );

    zox_system(
        SkillToggleSystem,
        EcsOnUpdate,
        [in] users.Activate,
        [out] skills.SkillActive,
        [none] Aura
    );

    /*zox_system(
        SkillActivateSystem,
        EcsOnUpdate,
        [in] users.Activate,
        [out] skills.SkillActive,
        [none] skills.Melee
    );
    zox_system(
        SkillWarmupSystem,
        EcsOnUpdate,
        [in] users.WarmupState,
        [out] skills.SkillActive
    );*/

    zox_filter(
        characters,
        [in] combat.Dead,
        [in] transforms3.Position3D,
        [out] hierarchys.Children,
        [out] stats.DotLinks
    );

    zox_system_ctx_1(
        DamageAuraSystem,
        zoxp_mainthread,
        characters,
        [in] users.UserLink,
        [in] SkillActive,
        [in] SkillDamage,
        [in] SkillRange,
        [in] colorz.Color,
        [none] Aura
    );
    zox_system(
        DamageAuraRemoveSystem,
        EcsOnUpdate,
        [in] transforms3.Position3D,
        [out] stats.DotLinks,
        [out] hierarchys.Children
    );
    zox_system(
        DotsSystem,
        EcsOnUpdate,
        [in] users.UserLink,
        [in] users.SpawnerLink,
        [in] SkillDamage,
        [none] Poison
    );

    // TODO: split into sound, resource and damage systems
    zox_system_1(
        MeleeSystem,
        zoxp_queue_add,
        [in] users.UserLink,
        [in] skills.SkillDamage,
        [in] skills.SkillDamageMax,
        [in] skills.SkillRange,
        [in] skills.SkillResourceLink,
        [in] skills.SkillCost,
        [in] users.Activate,
        [none] skills.Melee
    );

    zox_system_1(
        AuraSoundSystem,
        EcsOnUpdate,
        [in] users.Activate,
        [none] skills.Aura
    );


    zox_system_1(
        CharacterSkillsSpawnSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [out] skills.SkillLinks
        // [none] !players.PlayerLink
    );
}