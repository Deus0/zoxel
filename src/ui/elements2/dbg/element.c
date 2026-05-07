 entity zox_dbg_element;

void zox_dbg_spawn_element(ecs *world, ClickEventData data) {
    /*if (keycode != zox_key_g) {
        return;
    }*/

    entity player = dbg_player;
    zox_log("Testing [element]: %lu", zox_dbg_element);
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
    // zox_dbg_element = spawn_element(world, sdata);
    entity element = spawn_element3(world, prefab_element_textured, float2_half, position, int2_single(size), int2_single(size), default_fill_color_icon, default_outline_color_icon);
    zox_set(element, Layer2D, { 1 });
    zox_set(element, CanvasLink, { canvas });
    // zox_set(element, ParentLink, { canvas });
    zox_set(canvas, WindowToTop, { element });
    zox_set_parent(world, element, canvas);
    zox_dbg_element = element;
}
