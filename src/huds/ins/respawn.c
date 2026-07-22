entity spawn_respawn_ui(ecs* world, entity player, entity canvas) {
    if (!zox_valid(canvas)) {
        return 0;
    }
    color fill = (color) { 88, 0, 0, 44 };
    color outline = (color) { 255, 0, 0, 88 };
    entity prefab = prefab_text;
    const char* text = "Respawning";
    byte font_size = ui_scale * 24;
    byte2 padding = byte2_single(1 * ui_scale);
    entity e = spawn_text(world, prefab, canvas, int2_zero, float2_half, font_size, zox_alignment_centre, padding, text, fill, outline);
    zox_set_unique_name(e, "respawn_ui");
    zox_add_tag(e, RespawnUI);
    zox_set(e, Layer2D, { get_game_overlay_layer() });
    return e;
}
