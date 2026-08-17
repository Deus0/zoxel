entity dbg_render_texture_character = 0;
extern entity get_linked_character(ecs*, entity);

void zox_tst_render_texture_character(ecs *world, ClickEventData data) {
    // byte is_camera_filtering = 1;
    // float downscale = 1;
    // byte layer = max_layers2D - 10;
    if (zox_valid(dbg_render_texture_character)) {
        zox_log("Deleting [dbg_render_texture_character]");
        zox_delete(dbg_render_texture_character);
        dbg_render_texture_character = 0;
        return;
    }
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    entity character = get_linked_character(world, player);
    if (!zox_valid(character)) {
        zox_log("No character to render");
        return;
    }
    entity canvas = get_linked_canvas(world, player);
    int padding = 40;
    int2 size = int2_single(512);
    // int2 tsize = int2_scale1(lsize, 1 / downscale);
    int2 position = (int2) { -size.x / 2, -size.y / 2 };
    position.x -= padding;
    position.y -= padding;
    float2 anchor = float2_top_right; // (float2) { 1, 1 };
    byte header_font_size = 8 * ui_scale;
    byte2 header_padding = (byte2) { 10 * ui_scale, 4 * ui_scale };
    entity3 e2 = spawn_window(world, prefab_window, prefab_body, "Character", canvas, position, size, anchor, header_font_size, header_padding, NULL);
    entity e = e2.x;
    entity body = e2.z;
    spawn_render_frame_at(world, body, float2_centre, int2_zero, size, character);
    dbg_render_texture_character = e;
}
