#include "statbar.c"
#include "labels.c"
#include "icon_labels.c"
#include "healthbars.c"
#include "tooltip.c"
#include "players.c"

void define_systems_stats_ui(ecs *world) {
    zox_system(
        StatbarSystem,
        EcsOnUpdate,
        [in] StatLink,
        [out] elements.ElementBar,
        [none] Statbar
    );
    zox_system(
        StatTextSystem,
        EcsOnUpdate,
        [in] StatLink,
        [out] texts.TextData,
        [out] texts.TextDirty,
        [none] StatsLabel
    );
    zox_system(
        StatIconLabelSystem,
        EcsOnUpdate,
        [in] slots.DataLink,
        [out] texts.TextData,
        [out] texts.TextDirty,
        [none] elements2.Label
    );
    zox_system_1(
        HealthbarSpawnerSystem,
        EcsOnUpdate,
        [in] combat.CombatState,
        [out] elements.ElementLinks
    );
    zox_system(
        StatIconTooltipSystem,
        EcsOnUpdate,
        [in] interaction.SelectState,
        [in] slots.DataLink,
        [none] elements2.Icon
    );
    zox_system_1(
        PlayerStatspanelSystem,
        EcsOnUpdate,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] layouts2.CanvasLink,
        [none] players.Player
    );
}
