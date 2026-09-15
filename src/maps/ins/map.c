entity spawn_map(ecs* world, entity canvas, entity player, entity terrain) {
    int2 tunk_position = int2_zero;
    entity camera = zox_get_link(world, player, CameraLink);
    if (zox_valid(camera)) {
        tunk_position = zox_getv(camera, StreamPosition2);
    }
    const char* header_text = "Map";
    int2 size = int2_single(120 * ui_scale);
    // Margin from canvas corner
    int2 position = int2_zero;
    float2 position_anchor = float2_half;
    byte header_font_size = 8 * ui_scale;
    byte2 header_padding = (byte2) { 10 * ui_scale, 4 * ui_scale };
    entity3 e3 = spawn_window(
        world,
        prefab_window,
        prefab_body,
        header_text,
        canvas,
        position,
        size,
        position_anchor,
        header_font_size,
        header_padding,
        &on_closed_taskbar_window);
    entity e = e3.x;
    zox_add(e, MenuMap);
    zox_add(e, Map);
    zox_link(world, e, PlayerLink, player);
    zox_link(world, e, TerrainLink, terrain);
    zox_setv(e, MapPosition, tunk_position);
    // int zoom = int_min(bigmap_zoom, terrain_lod_far);
    // zox_log("Map Zoom [%i] from [%i, %i]", zoom, bigmap_zoom, terrain_lod_far);
    byte zoom = terrain_lod_far;
    zox_setv(e, MapZoom, zoom);
    zox_setv(e, Alpha, bigmap_alpha);
    return e;
}

entity spawn_player_menu_map(ecs* world, entity player) {
    entity canvas = zox_get_link(world, player, CanvasLink);
    entity game = zox_get_parent(world, player);
    entity realm = zox_get_link(world, game, RealmLink);
    entity terrain = zox_get_link(world, realm, TerrainLink);
    return spawn_map(world, canvas, player, terrain);
}
