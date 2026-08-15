entity dbg_bar2;

void zox_dbg_spawn_bar2(ecs *world, ClickEventData data) {
    byte font_size = 16;
    byte2 padding = (byte2) { 8, 6 };
    int2 size = (int2) { font_size * 12 + padding.x * 2, font_size + padding.y * 2 };
    // Colors
    color fill = (color) { 90, 8, 12, 220 };          // dark blood red background
    // color outline = (color) { 35, 5, 5, 255 };        // almost black red border
    color fill2 = (color) { 40, 220, 80, 240 };       // healthy green fill
    // color outline2 = (color) { 170, 255, 170, 255 };  // light green edge glow
    // color font_fill = (color) { 255, 235, 120, 255 }; // gold numbers
    // color font_outline = (color) { 60, 20, 0, 255 };  // dark orange shadow
    if (zox_valid(dbg_bar2)) {
        zox_log("+ Deleting [dbg_bar2]");
        zox_delete(dbg_bar2);
        dbg_bar2 = 0;
        return;
    }
    zox_log("+ Testing [dbg_bar2]");
    entity camera = zox_getv(dbg_player, CameraLink);
    entity canvas = zox_getv(dbg_player, CanvasLink);
    if (!zox_valid(camera)) {
        return;
    }
    entity2 spawns = spawn_bar2(
        world,
        prefab_bar2,
        canvas,
        int2_zero,
        size,
        float2_zero,
        font_size,
        padding,
        fill,
        fill2);
    dbg_bar2 = spawns.x;
    entity bar = spawns.y;
    zox_lerp_float_ins(bar, BarLevel, 1, 0, 5);
}
