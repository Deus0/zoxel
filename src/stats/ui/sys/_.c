#include "statbar.c"
#include "labels.c"
#include "icon_labels.c"
#include "healthbars.c"
#include "tooltip.c"
#include "players.c"

void zox_systems_stats_ui(ecs *world) {
    zox_system(
        StatbarSystem,
        zoxp_update,
        [out] ui.BarLevel,
        [none] Statbar
    );
    zox_system(
        StatTextSystem,
        zoxp_update,
        [out] texts.TextData,
        [none] texts.Text,
        [none] StatsLabel,
    );
    zox_system(
        StatIconLabelSystem,
        zoxp_update,
        [in] slots.DataLink,
        [out] texts.TextData,
        [none] texts.Text,
        [none] elements2.Label,
    );
    zox_system(
        StatIconTooltipSystem,
        zoxp_update,
        [in] interaction.SelectState,
        [in] slots.DataLink,
        [none] elements2.Icon
    );
    zox_system_1(
        HealthbarSpawnerSystem,
        zoxp_spawn,
        [in] combat.CombatState,
        [out] ui.ElementLinks
    );
}

void zox_events_stats_ui(ecs* world) {
    zox_muter(prefab_player, PlayerStateEvent, player_event);
    add_to_PlayerStateEvent(player_event, player_state_stats_ui);
}
