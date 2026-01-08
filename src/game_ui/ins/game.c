// TODO: Refactor these modules onto the game ui
extern entity spawn_menu_game_stats(ecs *world, const entity menu_game, const entity player, Children *children);

entity spawn_menu_game(
    ecs *world,
    const entity prefab,
    const entity player,
    const entity character
) {
    if (!zox_valid(character) || !zox_valid(player)) {
        zox_log_error("[!spawn_menu_game] Issue with character or player, invalid");
        return 0;
    }

    byte size = (crosshair_pixel_size.x / 4) * ui_scale;

    const entity canvas = zox_get_value(player, CanvasLink)
    const int2 canvas_size = zox_get_value(canvas, LayoutSize)
    // make layout2 instead of element_invisible
    const entity e = spawn_layout2_on_canvas(
        world,
        prefab,
        canvas,
        int2_zero,
        canvas_size,
        float2_half);
    zox_name("menu_game")
    Children children = (Children) { 0 };

    const entity crosshair = spawn_crosshair(
        world,
        (LayoutParentData) {
            .e = canvas,
            // .size = canvas_size,
        },
        (LayoutParentData) {
            .e = e,
            .position = int2_zero,
            // .size = canvas_size,
        },
        (ElementSpawnData) {
            .prefab = prefab_crosshair,
            .layer = 1,
            .anchor = float2_half,
            .position = int2_zero,
            .size = int2_single(size),
            .texture_size = int2_single(size),
        });

    add_to_Children(&children, crosshair);

    spawn_menu_game_stats(world, e, player, &children);

    zox_set_ptr(e, Children, children);

    // link to character
    zox_muter(character, ElementLinks, elements);
    add_to_ElementLinks(elements, e);
    zox_set(e, ElementHolder, { character });

    local_menu_game = e;
    return e;
}

void dispose_menu_game(
    ecs *world,
    const entity player
) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    find_child_with_tag(canvas, MenuGame, game_ui);
    if (game_ui) {
        zox_delete(game_ui)
    }
    dispose_menu_game_touch(world, player);
}



// called from game state changes
entity spawn_in_game_ui(ecs *world, const entity player) {
    if (!zox_has(player, DeviceMode) || !zox_has(player, CanvasLink)) {
        zox_log_error("Invalid player in [spawn_in_game_ui]")
        return 0;
    }
    zox_geter_value(player, DeviceMode, byte, device_mode);
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter(player, CharacterLink, character);
    entity e = spawn_menu_game(
        world,
        prefab_menu_game,
        player,
        character->value
    );
    byte is_touch = device_mode == zox_device_mode_touchscreen;
#ifdef zoxel_mouse_emulate_touch
    is_touch = 1;
#endif
    if (is_touch) {
        spawn_in_game_ui_touch(world, player, canvas);
    }
    return e;
}

extern entity spawn_player_menu_actions(ecs *world, const entity player);

void spawn_player_game_ui(
    ecs *world,
    const entity player
) {
    spawn_in_game_ui(world, player);
    spawn_player_menu_actions(world, player);
}
