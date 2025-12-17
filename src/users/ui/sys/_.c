#include "user_icon_tooltip.c"
#include "user_icon_click.c"

void define_systems_users_ui(ecs* world) {
    zox_system(
        UserIconTooltipSystem,
        EcsOnUpdate,
        [in] elements.SelectState,
        [in] users.UserDataLink,
        [in] layouts2.CanvasLink,
        [in] elements.TooltipEvent,
        [none] UserIcon
    );
    zox_system(
        UserIconClickSystem,
        EcsPreUpdate,
        [in] elements.ClickState,
        [in] elements2.IconType,
        [in] elements2.IconIndex,
        [out] users.UserDataLink,
        [none] UserIcon
    );
}