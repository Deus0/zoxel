#include "statbar.c"
#include "stat_text.c"
#include "label.c"
#include "healthbar_spawner.c"
#include "tooltip.c"

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
        [in] u.i.containers.DataLink,
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
        [in] u.i.containers.DataLink,
        [none] elements2.Icon
    );
}
