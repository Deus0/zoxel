#include "label.c"
#include "tooltip.c"

void zox_define_systems_quests_ui(ecs* world) {
    zox_system(
        QuestIconTooltipSystem,
        zoxp_update,
        [in] interaction.SelectState,
        [in] slots.DataLink,
        [none] elements2.Icon
    );
    zox_system(
        QuestIconLabelSystem,
        zoxp_update,
        [in] slots.DataLink,
        [out] texts.TextData,
        [out] texts.TextDirty,
        [none] elements2.Label
    );
}
