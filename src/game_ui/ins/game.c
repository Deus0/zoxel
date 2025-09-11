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
    Children children = (Children) { 0, NULL };

    const entity crosshair = spawn_crosshair(
        world,
        (LayoutParentData) {
            .e = canvas,
            .size = canvas_size,
        },
        (LayoutParentData) {
            .e = e,
            .position = int2_zero,
            .size = canvas_size,
        },
        (ElementSpawnData) {
            .prefab = prefab_crosshair,
            .layer = 1,
            .anchor = float2_half,
            .position = int2_zero,
            .size = crosshair_pixel_size,
            .texture_size = crosshair_texture_size,
        });

    add_to_Children(&children, crosshair);

    spawn_menu_game_stats(world, e, player, &children);

    zox_set_ptr(e, Children, children)

    // link to character
    zox_muter(character, ElementLinks, elementLinks);
    add_to_ElementLinks(elementLinks, e);
    zox_set(e, ElementHolder, { character })

    local_menu_game = e;
    return e;
}

void dispose_menu_game(
    ecs *world,
    const entity player
) {
    const entity canvas = zox_get_value(player, CanvasLink)
    find_child_with_tag(canvas, MenuGame, game_ui)
    if (game_ui) {
        zox_delete(game_ui)
    }
    dispose_menu_game_touch(world, player);
}