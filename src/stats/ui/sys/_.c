#include "statbar.c"
#include "labels.c"
#include "icon_labels.c"
#include "healthbars.c"
#include "tooltip.c"
#include "players.c"

void define_systems_stats_ui(ecs *world) {
    zox_system(
        StatbarSystem,
        zoxp_update,
        [in] StatLink,
        [out] ui.BarLevel,
        [none] Statbar
    );
    zox_system(
        StatTextSystem,
        zoxp_update,
        [in] StatLink,
        [out] texts.TextData,
        [out] texts.TextDirty,
        [none] StatsLabel
    );
    zox_system(
        StatIconLabelSystem,
        zoxp_update,
        [in] slots.DataLink,
        [out] texts.TextData,
        [out] texts.TextDirty,
        [none] elements2.Label
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
    zox_system_1(
        PlayerStatspanelSystem,
        zoxp_spawn,
        [in] players.PlayerStateDirty,
        [in] players.PlayerState,
        [in] characters.CharacterLink,
        [in] layouts.CanvasLink,
        [none] players.Player
    );
}
