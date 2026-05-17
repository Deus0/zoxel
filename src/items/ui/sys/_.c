#include "label.c"
#include "tooltip.c"

void define_systems_items_ui(ecs *world) {
    zox_system(
        ItemIconLabelSystem,
        EcsOnUpdate,
        [in] u.i.containers.SlotLink,
        [out] texts.TextData,
        [out] texts.TextDirty,
        [none] elements2.Label
    );
    zox_system(
        ItemIconTooltipSystem,
        EcsOnUpdate,
        [in] elements.SelectState,
        [in] u.i.containers.DataLink,
        [none] elements2.Icon
    );
}
