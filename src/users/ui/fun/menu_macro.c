#define zox_user_menu_functions_minimal(Name, Name2, name, name2)\
\
entity spawn_prefab_menu_##name(ecs *world, entity prefab) {\
    zox_prefab_child(prefab); \
    zox_prefab_name("menu_"#name); \
    zox_add_tag(e, Menu##Name); \
    return e;\
}

#define zox_user_menu_functions(Name, Name2, name, name2)\
    \
    zox_user_menu_functions_minimal(Name, Name2, name, name2)\
    \
    entity spawn_player_menu_##name(ecs *world, entity player) {\
        \
        entity character = zox_get_value(player, CharacterLink)\
        entity canvas = zox_get_value(player, CanvasLink)\
        \
        if (!zox_valid(canvas)) {\
            return 0;\
        }\
        \
        int2 canvas_size = zox_get_value(canvas, LayoutSize)\
        \
        SpawnWindowUsers data = get_default_spawn_window_users_data(world, prefab_menu_##name, character, canvas, canvas_size);\
        \
        data.header_zext.text = #Name;\
        data.element.prefab = prefab_menu_##name;\
        data.icon.prefab = prefab_icon_##name2;\
        data.window.user_links_id = zox_id(Name2##Links);\
        FrameTextureData texture = (FrameTextureData) { \
            .fill_color = window_fill, \
            .outline_color = window_outline \
        }; \
        \
        return spawn_window_users_id(world, data, texture, 0, NULL);\
    }

// Example: zox_user_menu_functions(Skills, Skill, skills, skill)
