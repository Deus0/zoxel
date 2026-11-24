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
    byte2 padding = (byte2) {
        8 * ui_scale,
        4 * ui_scale
    };
    color fill = button_fill;
    fill.a = 177;
    color outline = button_outline;
    color font_fill = button_font_fill;
    color font_outline = button_font_outline;
    // 3D huds: the same label on a root ahead of the head (huds3_spawn_root), its 3D twin, a canvas3 its size
    entity parent = canvas;
    if (zox_huds3D) {
        const int2 size = { (int) strlen(text) * font_size + padding.x * 2, font_size + padding.y * 2 };
        parent = huds3_spawn_root(world, player, canvas, size, huds3_label_height);
    }
    const entity label = spawn_label(
        world,
        prefab_label2t,
        parent,
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
    const entity e = zox_huds3D ? parent : label;
    zox_set_unique_name(e, "menu_start_label");
    zox_add(e, MenuStart);
    if (!zox_huds3D) {
        zox_link(world, e, PlayerLink, player);     // a 3D root's linked already
    }
    zox_link(world, player, MenuStart, e);
    {
        // entity e2 = spawn_test_vox(world, player);
        // zox_set(e2, GenerateModel, { zox_generate_model_run });
        // zox_set_parent(world, e2, e);
    }
    return e;
}
