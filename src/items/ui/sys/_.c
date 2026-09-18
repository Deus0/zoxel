#include "label.c"
#include "tooltip.c"

void zox_systems_items_ui(ecs *world) {
    zox_system(
        ItemIconTooltipSystem,
        zoxp_update,
        [in] interaction.SelectState,
        [in] slots.DataLink,
        [none] elements2.Icon
    );
    zox_system(
        ItemIconLabelSystem,
        zoxp_update,
        [in] slots.DataDirty,
        [in] slots.SlotLink,
        [out] texts.TextData,
        [none] texts.Text,
        [none] elements2.Label
    );
}
