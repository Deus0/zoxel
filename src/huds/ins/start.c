// Spawn a games Start / Title Menu
entity spawn_menu_start(ecs* world, entity player, entity canvas) {
    char* label_start = "Cube is End";
    byte layer = 1;
    byte font_size = 18 * ui_scale;
    byte2 padding = (byte2) { 8 * ui_scale, 4 * ui_scale };
    color fill = button_fill;
    fill.a = 177;
    entity e = spawn_layout2(world, prefab_layout2, canvas, int2_zero, int2_zero, float2_half, layer);
    zox_name("menu_start");
    zox_add(e, MenuStart);
    zox_setv(e, PlayerLink, player);
    {
       entity e2 = spawn_label(world, prefab_label_textured, e, int2_zero, float2_half, padding, label_start, font_size, zox_alignment_centre, layer, fill, button_outline, button_font_fill, button_font_outline);
       zox_set_unique_name(e2, "menu_start_label");
    }
    {
        entity e2 = spawn_test_vox(world, player);
        // zox_set(e2, GenerateModel, { zox_generate_model_run });
        zox_set_parent(world, e2, e);
    }
    return e;
}

void spawn_all_players_start_ui(ecs *world) {
    zox_logv("Spawning Player Start Menus [%i]", players_playing);
    for (int i = 0; i < players_playing; i++) {
        entity player = zox_players[i];
        entity canvas = zox_getv(player, CanvasLink);
        if (!zox_valid(canvas)) {
            continue;
        }
        zox_logv("Player [%s] | canvas [%s]", zox_getn(player), zox_getn(canvas));
        spawn_menu_start(world, player, canvas);
    }
}
