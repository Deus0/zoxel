#include "regen.c"
#include "death.c"
#include "death_animation.c"
#include "level_up.c"
#include "experience.c"
#include "character.c"
#include "world_labels.c"
#include "character_player.c"

void zox_systems_stats(ecs *world) {
    zox_system(
        stat_regen_system,
        zoxp_update,
        [in] stats.StatValue,
        [none] stats.Stat,
        [none] stats.StatRegen,
        [none] !stats.StatDisabled,
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
        death_animation_system,
        zoxp_update,
        [out] animations.AnimationState,
        [out] animations.AnimationStart,
        [none] core.DeathDirty,
    );
    zox_system_1(
        experience_system,
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
        character_stats_system,
        zoxp_spawn,
        [none] characters.Character,
        [none] core.Initialize,
        [none] !players.PlayerCharacter
    );
    zox_system_1(
        player_character_stats_system,
        zoxp_spawn,
        [none] characters.Character,
        [none] core.Initialize,
        [none] players.PlayerCharacter,
    );
    // spawns and deletes name labels near the players' cameras
    zox_filter(name_label_players,
        [none] players.Player
    );
    zox_system_ctx_1(
        character_name_labels_system,
        zoxp_spawn,
        name_label_players,
        [in] core.ZoxName,
        [in] transforms3.Position3D,
        [none] SpawnNameLabel
    );
}
