#include "icon_label.c"

void define_systems_items_ui(ecs *world) {
    zox_system(
        ItemIconLabelSystem,
        EcsOnUpdate,
        [in] ItemLink,
        [out] texts.TextData,
        [out] texts.TextDirty,
        [none] ItemIconLabel
    );
}