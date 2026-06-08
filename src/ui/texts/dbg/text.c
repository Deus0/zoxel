entity zox_dbg_text;

void zox_tst_spawn_text2(ecs* world, ClickEventData data) {
    entity player = dbg_player;
    zox_log("Testing [spawn_text]: %lu", zox_dbg_text);
    if (zox_valid(zox_dbg_text)) {
        zox_delete(zox_dbg_text);
        zox_dbg_text = 0;
        return;
    }
    zox_geter_value(player, CanvasLink, entity, canvas);
    if (!zox_valid(canvas)) {
        return;
    }
    zox_log("   + spawning zox_dbg_text on player %s on canvas %s", zox_get_name(player), zox_get_name(canvas));
    entity prefab = prefab_text;
    const char* text = "Testing";
    byte font_size = ui_scale * 32;
    byte2 padding = byte2_single(1 * ui_scale);
    entity e = spawn_text(world, prefab, canvas, int2_zero, float2_half, font_size, zox_alignment_centre, padding, text, window_fill, window_outline);
    zox_set_unique_name(e, "zox_tst_text");
    zox_dbg_text = e;
}
