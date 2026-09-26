entity spawn_player_menu_crafting(ecs* world, entity player) {
    entity canvas = zox_get_link(world, player, CanvasLink);
    entity character = zox_get_link(world, player, CharacterLink);
    entity craftspace = zox_get_child_by_id(world, character, zox_id(Craftspace));
    byte2 cells_size = byte2_single(3);
    byte label_font_size = 5 * ui_scale;
    float2 position_anchor = float2_half;
    int2 position = int2_zero;
    entity frame_id = zox_id(ItemFrame);
    entity e = spawn_datagrid_slots(
        world,
        prefab_window,
        prefab_frame,
        prefab_icon,
        prefab_label2,
        label_font_size,
        canvas,
        character,
        craftspace,
        cells_size,
        "Craft",
        color_white,
        color_white,
        position_anchor,
        position,
        frame_id,
        0);
    zox_add(e, MenuCrafting);
    return e;
 }


/*entity spawn_player_menu_crafting(ecs* world, entity player) {
    entity canvas = zox_get_link(world, player, CanvasLink);
    entity character = zox_get_link(world, player, CharacterLink);
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);
    if (!zox_has(character, CraftLinks)) {
        return 0;
    }
    SpawnWindowUsers data = get_default_spawn_window_users_data(
        world,
        prefab_menu_crafting,
        character,
        canvas,
        canvas_size
    );
    data.header_zext.text = "Craft";
    data.element.prefab = prefab_menu_crafting;
    data.window.user_links_id = zox_id(CraftLinks);
    data.frame.texture.fill_color = default_fill_color_frame_crafting;

    FrameTextureData texture = (FrameTextureData) {
        .fill_color = window_fill,
        .outline_color = window_outline
    };

    zox_geter(character, CraftLinks, links);
    entity3 spawns[links->length];
    return 0;
}
*/
