#include "regen.c"
#include "death.c"
#include "death_animation.c"
#include "level_up.c"
#include "experience.c"
#include "character.c"
#include "world_labels.c"
#include "character_player.c"
// realm_clear_system(StatLinks);

void zox_systems_stats(ecs *world) {
    // realm_clear_systemd(stats, StatLinks);
    // debuff system here, skills will add debuffs
    zox_system(
        DeathSystem,
        zoxp_update,
        [in] stats.StatValue,
        [none] stats.StatHealth
    );
    zox_system(
        StatRegenSystem,
        zoxp_update,
        [in] stats.StatValueMax,
        [out] stats.StatValue,
        [out] stats.StatDirty,
        [none] stats.StatState
    );
    zox_system(
        DeathAnimationSystem,
        zoxp_update,
        [in] combat.Dead,
        [out] combat.DiedTime,
        [out] animations.AnimationState,
        [out] animations.AnimationStart
    );
    zox_system_1(
        ExperienceSystem,
        zoxp_spawn,
        [in] combat.Dead,
        [in] combat.LastDamager
    );
    zox_system_1(
        LevelUpSystem,
        zoxp_spawn,
        [out] stats.StatValue,
        [out] stats.ExperienceValue,
        [out] stats.ExperienceMax,
        [out] stats.StatDirty,
        [none] stats.StatLevel
    );
    zox_system_1(
        CharacterStatsSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [none] !players.PlayerLink
    );
    zox_system_1(
        PlayerCharacterStatsSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [none] players.PlayerLink
    );
    zox_system_1(
        CharacterNameLabelsSystem,
        zoxp_spawn,
        // [in] characters.GenerateCharacter,
        [in] core.ZoxName,
        [out] ui.ElementLinks,
        [none] SpawnNameLabel
    );
}
