#include "tooltip.c"
#include "click.c"

void define_systems_users_ui(ecs* world) {
    zox_system(
        UserIconTooltipSystem,
        EcsOnUpdate,
        [in] elements.SelectState,
        [in] u.i.containers.DataLink,
        [in] elements.TooltipEvent,
        [none] UserIcon
    );
    zox_system(
        UserIconClickSystem,
        EcsPreUpdate,
        [in] elements.ClickState,
        [in] elements2.IconType,
        [in] elements2.IconIndex,
        [out] u.i.containers.DataLink,
        [none] UserIcon
    );
}
