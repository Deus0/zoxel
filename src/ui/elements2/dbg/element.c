 entity zox_dbg_element;

void zox_tst_key_down_element(ecs *world, int32_t keycode) {

    if (keycode != zox_key_h) {
        return;
    }

    entity player = dbg_player;

    zox_log("Testing [element]: %lu", zox_tst_key_down_text);

    if (zox_dbg_element) {
        zox_delete(zox_dbg_element);
        zox_dbg_element = 0;
        return;
    }


    zox_geter_value(player, CanvasLink, entity, canvas);

    if (!zox_valid(canvas)) {
        return;
    }

    zox_log("   + spawning [zox_dbg_element] on player %s on canvas %s", zox_get_name(player), zox_get_name(canvas));

    byte size = ui_scale * 32;
    int2 position = (int2) { size, size * 2 };

    ElementSpawn sdata = {
        .canvas = { canvas },
        .parent = { canvas },
        .element = {
            .prefab = prefab_element_textured,
            .layer = 1,
            .anchor = float2_half,
            .position = position,
            .size = int2_single(size),
            .texture_size = int2_single(size),
        },
        .texture = {
            .fill_color = default_fill_color_icon,
            .outline_color = default_outline_color_icon,
        }
    };

    zox_dbg_element = spawn_element(world, sdata);
}