#include "regen.c"
#include "death.c"
#include "death_animation.c"
#include "level_up.c"
#include "experience.c"
#include "character.c"
#include "world_labels.c"
#include "character_player.c"
realm_clear_system(StatLinks);

void define_systems_stats(ecs *world) {

    realm_clear_systemd(stats, StatLinks);

    // debuff system here, skills will add debuffs
    zox_system(
        DeathSystem,
        EcsOnUpdate,
        [in] stats.StatValue,
        [in] users.UserLink,
        [none] stats.HealthStat
    );
    zox_system(
        StatRegenSystem,
        EcsOnUpdate,
        [in] users.UserLink,
        [in] stats.StatValueMax,
        [out] stats.StatValue,
        [out] stats.StatDirty,
        [none] stats.StatState
    );
    zox_system(
        DeathAnimationSystem,
        EcsOnUpdate,
        [in] combat.Dead,
        [out] combat.DiedTime,
        [out] physics.DisableMovement,
        [out] animations.AnimationState,
        [out] animations.AnimationStart
    );
    zox_system_1(
        ExperienceSystem,
        EcsOnUpdate,
        [in] combat.Dead,
        [in] stats.StatLinks,
        [in] combat.LastDamager
    );
    zox_system_1(
        LevelUpSystem,
        EcsOnUpdate,
        [in] users.UserLink,
        [out] stats.StatValue,
        [out] stats.ExperienceValue,
        [out] stats.ExperienceMax,
        [out] stats.StatDirty,
        [none] stats.StatLevel
    );
    zox_system_1(
        CharacterStatsSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [out] stats.StatLinks
    );
    zox_system_1(
        PlayerCharacterStatsSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [out] stats.StatLinks,
        [none] players.PlayerLink
    );
    zox_system_1(
        CharacterNameLabelsSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        [in] core.ZoxName,
        [in] stats.StatLinks,
        [out] elements.ElementLinks
    );
}
