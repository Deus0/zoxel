entity dbg_bar3;

void zox_dbg_spawn_bar3(ecs *world, ClickEventData data) {
    float distance = 0.25f;
    // float scale = randf_range(0.04f, 0.08f);
    byte font_size = 16;
    byte2 padding = (byte2) { 8, 6 };
    int2 bar_size = (int2) { font_size * 12 + padding.x * 2, font_size + padding.y * 2 };
    // Colors
    color fill = (color) { 90, 8, 12, 220 };          // dark blood red background
    color outline = (color) { 35, 5, 5, 255 };        // almost black red border
    color fill2 = (color) { 40, 220, 80, 240 };       // healthy green fill
    color outline2 = (color) { 170, 255, 170, 255 };  // light green edge glow
    color font_fill = (color) { 255, 235, 120, 255 }; // gold numbers
    color font_outline = (color) { 60, 20, 0, 255 };  // dark orange shadow
    if (zox_valid(dbg_bar3)) {
        zox_log("+ Deleting [dbg_bar3]");
        zox_delete(dbg_bar3);
        dbg_bar3 = 0;
        return;
    }
    zox_log("+ Testing [popup3]");
    entity camera = zox_getv(dbg_player, CameraLink);
    if (!zox_valid(camera)) {
        return;
    }
    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float3 spawn_position = move_along_direction(position, rotation, -distance);
    entity3 spawns = spawn_bar3(world, spawn_position, zox_ui_scale3, bar_size, font_size, fill, outline, fill2, outline2, font_fill, font_outline, 0, 0);
    entity bar = spawns.y;
    dbg_bar3 = spawns.x;
    zox_lerp_float_ins(bar, BarLevel, 1, 0, 5);
}
