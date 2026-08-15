 entity zox_dbg_element_canvas;

 void zox_dbg_spawn_element_canvas(ecs *world, ClickEventData data) {
     entity player = dbg_player;
     zox_log("Testing [zox_dbg_element_canvas]: %lu", zox_dbg_element_canvas);
     if (zox_dbg_element_canvas) {
         zox_delete(zox_dbg_element_canvas);
         zox_dbg_element_canvas = 0;
         return;
     }
     zox_geter_value(player, CanvasLink, entity, canvas);
     if (!zox_valid(canvas)) {
         return;
     }
     byte layer = 32;
     zox_log("Test Spawning [zox_dbg_element_canvas] on player %s on canvas %s", zox_get_name(player), zox_get_name(canvas));
     int2 canvas_size = zox_getv(canvas, LayoutSize);
     entity e = spawn_layout2(world, prefab_layout2, canvas, int2_zero, canvas_size, float2_half, layer);
     byte size = ui_scale * 32;
     int2 bounds = (int2) { canvas_size.x / 2 - size / 2, canvas_size.y / 2 - size / 2 };
     for (int i = -1; i <= 1; i+=2)
     for (int j = -1; j <= 1; j+=2)
     {
         int2 position = (int2) { i * bounds.x, j * bounds.y };
         entity e2 = spawn_uic(world, prefab_element_frame, e, float2_half, position, int2_single(size), int2_single(size), default_fill_color_icon, default_outline_color_icon);
         zox_set(e2, Layer, { layer + 1 });
     }
     zox_dbg_element_canvas = e;
 }
