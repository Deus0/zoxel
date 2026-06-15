// Spawn a games Start / Title Menu
entity spawn_menu_start(ecs* world, entity player, entity canvas) {
    char* label_start = "Cube is End"; // "Start";
    byte layer = game_overlay_layer + 1;
    byte font_size = 16 * ui_scale;
    byte font_thickness = ui_scale;
    byte2 padding = (byte2) { 8 * ui_scale, 4 * ui_scale };
    color fill = button_fill;
    fill.a = 177;
    entity e = spawn_layout2(world, prefab_layout2, canvas, int2_zero, int2_zero, float2_half, layer);
    zox_name("menu_start");
    zox_add_tag(e, MenuStart);
    zox_set(e, PlayerLink, { player });
    {
        spawn_label(world, prefab_label_textured, e, int2_zero, float2_half, padding, label_start, font_size, zox_alignment_centre, layer, fill, button_outline, button_font_fill, button_font_outline);
        // zox_set_parent(world, e2, e);
    }
    {
        entity e2 = zox_dbg_spawn_chunk3(world, 0);
        zox_set_parent(world, e2, e);
    }
    return e;
}

void spawn_all_players_start_ui(ecs *world) {
    zox_logv("Spawning Player Start Menus [%i]", players_playing);
    for (int i = 0; i < players_playing; i++) {
        entity player = zox_players[i];
        zox_geter_value(player, CanvasLink, entity, canvas);
        zox_logv("  - player [%s] | canvas [%s]", zox_get_name(player), zox_get_name(canvas));
        spawn_menu_start(world, player, canvas);
    }
}
