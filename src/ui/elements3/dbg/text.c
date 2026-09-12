entity dbg_text3;

void zox_dbg_spawn_label3(ecs *world, ClickEventData data) {
    float canvas_scale = 0.5f;
    float distance = 1.0f;
    const char* text = "Testel";
    byte font_size = 32;
    color fill = color_white;
    color outline = color_black;
    color font_fill = color_red;
    color font_outline = color_cyan;
    fill.a = 55;
    if (dbg_text3) {
        zox_log("+ Deleting [dbg_text3]");
        zox_delete(dbg_text3);
        dbg_text3 = 0;
        return;
    }
    entity camera = zox_get_link(world, dbg_player, CameraLink);
    if (!zox_valid(camera)) {
        return;
    }
    zox_log("+ Testing [dbg_element3]");
    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float3 spawn_position = move_along_direction(position, rotation, -distance);
    entity2 e2 = spawn_label3(
        world,
        text,
        font_size,
        fill,
        outline,
        font_fill,
        font_outline,
        zox_ui_scale3,
        0,
        0);
    zox_set_unique_name(e2.x, "dbg_label");
    zox_set_unique_name(e2.y, "dbg_label_text");
    zox_setv(e2.x, Position3D, spawn_position);
    zox_setv(e2.x, Rotation3D, rotation);
    zox_setv(e2.x, Scale1, canvas_scale);
    dbg_text3 = e2.x;
}
