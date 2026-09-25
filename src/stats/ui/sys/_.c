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
        [none] Statbar,
    );
    zox_system(
        StatTextSystem,
        zoxp_update,
        [out] texts.TextData,
        [none] texts.Text,
        [none] StatsLabel,
    );
    zox_system_1(
        HealthbarSpawnerSystem,
        zoxp_spawn,
        [in] combat.CombatState,
        [none] characters.Character,
    );
}
