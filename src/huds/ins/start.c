// Spawn a games Start / Title Menu
entity spawn_menu_start(
    ecs* world,
    entity player,
    entity canvas)
{
    if (!zox_valid(player)) {
        zox_loge("Player invalid in [spawn_menu_start]");
        return 0;
    }
    char* text = "Cube is End";
    byte layer = 1;
    byte font_size = 18 * ui_scale;
    byte2 padding = (byte2) { 8 * ui_scale, 4 * ui_scale };
    color fill = button_fill;
    fill.a = 177;
    color outline = button_outline;
    color font_fill = button_font_fill;
    color font_outline = button_font_outline;
    entity e;
    if (zox_huds3D) {
        if (!zox_has(player, CameraLink)) {
            zox_loge("Player Invalid Components in [spawn_menu_start]");
            return 0;
        }
        entity camera = zox_getv(player, CameraLink);
        if (!zox_valid(camera) ||
            !zox_has(camera, Position3D) ||
            !zox_has(camera, Rotation3D)
        ) {
            zox_loge("Camera Invalid in [spawn_menu_start]");
            return 0;
        }
        float3 position = zox_getv(camera, Position3D);
        float4 rotation = zox_getv(camera, Rotation3D);
        float canvas_scale = 0.06f;
        float distance = 1; // 0.5f;
        float3 spawn_position = move_along_direction(position, rotation, -distance);
        e = spawn_label3(
            world,
            text,
            font_size,
            fill,
            outline,
            font_fill,
            font_outline,
            zox_ui_scale3,
            0,
            0).x;
        zox_setv(e, Position3D, spawn_position);
        zox_setv(e, Rotation3D, rotation);
        zox_setv(e, Scale1, canvas_scale);
    } else {
       e = spawn_label(
           world,
           prefab_label2t,
           canvas,
           int2_zero,
           float2_half,
           padding,
           text,
           font_size,
           zox_alignment_centre,
           layer,
           fill,
           outline,
           font_fill,
           font_outline);
    }
    zox_set_unique_name(e, "menu_start_label");
    zox_add(e, MenuStart);
    zox_setv(e, PlayerLink, player);
    zox_link(world, player, MenuStart, e);
    {
        // entity e2 = spawn_test_vox(world, player);
        // zox_set(e2, GenerateModel, { zox_generate_model_run });
        // zox_set_parent(world, e2, e);
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
