 entity zox_dbg_text;

void zox_tst_key_down_text(ecs *world, int32_t keycode) {

    if (keycode != zox_key_h) {
        return;
    }

    entity player = dbg_player;

    zox_log("Testing [spawn_text]: %lu", zox_tst_key_down_text);

    if (zox_dbg_text) {
        zox_delete(zox_dbg_text);
        zox_dbg_text = 0;
        return;
    }


    zox_geter_value(player, CanvasLink, entity, canvas);

    if (!zox_valid(canvas)) {
        return;
    }

    zox_log("   + spawning zox_dbg_text on player %s on canvas %s", zox_get_name(player), zox_get_name(canvas));

    const char* label = "Testing";
    byte font_size = ui_scale * 32;

    SpawnZext textd = {
        .canvas = { .e = canvas },
        .parent = { .e = canvas },
        .zext = {
            .text = label,
            .font_size = font_size,
            .font_resolution = font_size,
            .font_thickness = 1,
            .font_outline_thickness = 4,
            .font_fill_color = window_fill,
            .font_outline_color = window_outline,
        },
        .element = {
            .prefab = prefab_zext,
            .anchor = float2_half,
        },
    };

    zox_dbg_text = spawn_zext(world, textd);
}