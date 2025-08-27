#include "melee.c"
#include "damage_aura.c"
#include "damage_aura_remove.c"
#include "dots.c"
#include "activate.c"
#include "toggle.c"
#include "aura_sound.c"
zox_declare_system_state_event(RealmSkills, GenerateRealm, zox_generate_realm_skills, spawn_realm_skills)

void define_systems_skills(ecs_world_t *world) {
    zox_define_system_state_event_1(
        RealmSkills,
        EcsOnLoad,
        realms.GenerateRealm,
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
    );*/
    zox_system(
        SkillWarmupSystem,
        EcsOnUpdate,
        [in] users.WarmupState,
        [out] skills.SkillActive
    );



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

    // todo: make a state system for SkillActive
    zox_system_1(
        MeleeSystem,
        zoxp_queue_add,
        [in] users.UserLink,
        [in] skills.SkillDamage,
        [in] skills.SkillDamageMax,
        [in] skills.SkillRange,
        [in] skills.SkillResourceLink,
        [in] skills.SkillCost,
        [out] skills.SkillActive,
        [none] skills.Melee
    );
    zox_system_1(
        AuraSoundSystem,
        EcsOnUpdate,
        [in] users.Activate,
        [none] skills.Aura
    );
}