// Spawn a games Start / Title Menu
entity spawn_menu_start(ecs* world, entity player, entity canvas) {
    byte layer = 3;
    byte font_size = 32 * ui_scale;
    byte font_thickness = ui_scale;
    byte2 margins = (byte2) { 8 * ui_scale, 4 * ui_scale };

    SpawnButton data = {
        .element = {
            .prefab = prefab_button,
            .layer = layer,
            .anchor = float2_half
        },
        .button = {
            .prefab_zext = prefab_zext,
            .fill = button_fill,
            .outline = button_outline,
        },
        .zext = {
            .text = label_start,
            .font_fill_color = button_font_fill,
            .font_outline_color = button_font_outline,
            .font_size = font_size,
            .font_thickness = font_thickness,
            .margins = margins,
        },
    };

    entity e = spawn_button(world, (LayoutParentData) { canvas }, (LayoutParentData) { canvas }, data.element, data.zext, data.button);
    zox_name("main_start");

    zox_add_tag(e, MenuStart);
    zox_set(e, PlayerLink, { player });
    zox_set(e, ClickEvent, { &button_event_menu_start });

    return e;
}

void spawn_all_players_start_ui(ecs *world) {
    zox_logv("Spawning Player Start Menus [%i]", players_playing)
    for (int i = 0; i < players_playing; i++) {
        entity player = zox_players[i];
        zox_geter_value(player, CanvasLink, entity, canvas);

        zox_logv("  - player [%s] | canvas [%s]", zox_get_name(player), zox_get_name(canvas));

        spawn_menu_start(world, player, canvas);
    }
}
