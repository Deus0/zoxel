#ifndef zoxm_users_ui
#define zoxm_users_ui

zox_tag(UserIcon);
zox_tag(UserIconWindow);
zox_tag(MenuStatus);
#include "data/icon_types.c"
#include "data/spawn_window_users.c"
#include "prefabs/prefabs.c"
#include "util/texture_swap.c"
#include "systems/user_icon_tooltip_system.c"
#include "systems/user_icon_click_system.c"
#include "util/menu_macro.c"

zox_begin_module(UsersUI)
    zoxd_tag(UserIcon);
    zoxd_tag(UserIconWindow);
    zoxd_tag(MenuStatus);
    zox_system(UserIconTooltipSystem, EcsOnUpdate,
            [in] elements.core.SelectState,
            [in] users.UserDataLink,
            [in] layouts2.CanvasLink,
            [in] elements.core.TooltipEvent,
            [none] UserIcon);
    zox_system(UserIconClickSystem, EcsPreUpdate,
            [in] elements.core.ClickState,
            [in] elements2.IconType,
            [in] elements2.IconIndex,
            [out] users.UserDataLink,
            [none] UserIcon);
    add_hook_spawn_prefabs(spawn_prefabs_users_ui);
zox_end_module(Users)

#endif