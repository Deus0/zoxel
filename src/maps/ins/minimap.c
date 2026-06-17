entity spawn_minimap(ecs* world, entity canvas, entity player, entity terrain) {
    int2 tunk_position = int2_zero;
    entity camera = zox_getv(player, CameraLink);
    if (zox_valid(camera)) {
        tunk_position = zox_getv(camera, StreamPosition2);
    }
    const char* header_text = "";
    int2 size = int2_single(60 * ui_scale);
    // Margin from canvas corner
    int2 position = (int2) { -16, -16 };
    float2 position_anchor = (float2) { 1, 1 };
    entity3 e3 = spawn_window(world, prefab_window, prefab_body, header_text, canvas, position, size, position_anchor, &on_closed_taskbar_window);
    entity e = e3.x;
    zox_add_tag(e, Map);
    zox_add_tag(e, Minimap);
    zox_set(e, PlayerLink, { player });
    zox_set(e, TerrainLink, { terrain });
    zox_set(e, MapPosition, { tunk_position });
    zox_set(e, MapZoom, { minimap_zoom });
    zox_set(e, Alpha, { minimap_alpha });
    return e;
}
