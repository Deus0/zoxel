#include "statbar.c"
#include "stat_text.c"
#include "icon_label.c"
#include "healthbar_spawner.c"

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
        [in] StatLink,
        [out] texts.TextData,
        [out] texts.TextDirty,
        [none] StatIconLabel
    );
    zox_system_1(
        HealthbarSpawnerSystem,
        EcsOnUpdate,
        [in] combat.CombatState,
        [in] stats.StatLinks,
        [out] elements.ElementLinks
    );
}