entity spawn_respawn_ui(
    ecs* world,
    entity canvas)
{
    if (!zox_valid(canvas)) {
        return 0;
    }
    color fill = (color) { 88, 0, 0, 144 };
    color outline = (color) { 255, 0, 0, 188 };
    entity prefab = prefab_text;
    const char* text = "Respawning";
    byte font_size = ui_scale * 24;
    byte2 padding = byte2_single(1 * ui_scale);
    entity e = spawn_text(
        world,
        prefab,
        canvas,
        int2_zero,
        float2_half,
        font_size,
        zox_alignment_centre,
        padding,
        text,
        fill,
        outline);
    zox_set_unique_name(e, "respawn_ui");
    zox_add(e, RespawnUI);
    zox_setv(e, Layer, get_game_overlay_layer());
    return e;
}
