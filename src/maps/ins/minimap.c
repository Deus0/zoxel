entity spawn_minimap(
    ecs* world,
    entity canvas,
    entity player,
    entity terrain)
{
    int2 tunk_position = int2_zero;
    entity camera = zox_get_link(world, player, Camera);
    if (zox_valid(camera)) {
        tunk_position = zox_getv(camera, StreamPosition2);
    }
    float2 position_anchor = float2_top_right;
    int2 size = int2_single(minimap_size * ui_scale);
    int2 position = int2_zero; //  (int2) { -16 * ui_scale, -16 * ui_scale };
    byte header_font_size = 4 * ui_scale;
    byte2 header_padding = (byte2) { 10 * ui_scale, 4 * ui_scale };
    const char* header_text = "";
    // Margin from canvas corner
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
    zox_set_unique_name(e, "minimap");
    zox_add(e, Map);
    zox_add(e, Minimap);
    zox_set(e, PlayerLink, { player });
    zox_set(e, TerrainLink, { terrain });
    zox_set(e, MapPosition, { tunk_position });
    zox_set(e, MapZoom, { minimap_zoom });
    zox_set(e, Alpha, { minimap_alpha });
    return e;
}
