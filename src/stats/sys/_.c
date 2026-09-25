#include "regen.c"
#include "death.c"
#include "death_animation.c"
#include "level_up.c"
#include "experience.c"
#include "character.c"
#include "world_labels.c"
#include "character_player.c"

void zox_systems_stats(ecs *world) {
    // debuff system here, skills will add debuffs
    zox_system(
        stat_regen_system,
        zoxp_update,
        [in] stats.StatValueMax,
        [out] stats.StatValue,
        [none] stats.Stat,
        [none] stats.StatState,
    );
    zox_system(
        pre_death_system,
        zoxp_update,
        [in] stats.StatValue,
        [none] stats.Stat,
        [none] stats.StatHealth,
    );
    zox_system(
        death_system,
        zoxp_update,
        [none] core.PreDeath
    );
    zox_system(
        post_death_system,
        zoxp_update,
        [none] core.DeathDirty
    );
    zox_system(
        DeathAnimationSystem,
        zoxp_update,
        [out] animations.AnimationState,
        [out] animations.AnimationStart,
        [none] core.DeathDirty,
    );
    zox_system_1(
        ExperienceSystem,
        zoxp_spawn,
        [none] core.Dead,
        [none] core.DeathDirty,
    );
    zox_system_1(
        level_up_system,
        zoxp_spawn,
        [out] stats.StatValue,
        [out] stats.ExperienceValue,
        [out] stats.ExperienceMax,
        [none] stats.Stat,
        [none] stats.StatLevel
    );
    zox_system_1(
        CharacterStatsSystem,
        zoxp_spawn,
        [none] characters.Character,
        [none] core.Initialize,
        [none] !players.PlayerCharacter
    );
    zox_system_1(
        PlayerCharacterStatsSystem,
        zoxp_spawn,
        [none] characters.Character,
        [none] core.Initialize,
        [none] players.PlayerCharacter,
    );
    zox_system_1(
        CharacterNameLabelsSystem,
        zoxp_spawn,
        [in] core.ZoxName,
        [out] ui.ElementLinks,
        [none] SpawnNameLabel
    );
}
