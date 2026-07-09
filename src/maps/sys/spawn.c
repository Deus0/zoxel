// Spawn a piece for now
//  A ui and just use Clone TextureData from TunkTexture
//  Link the map piece to a Tunk
// NOTE: Adds/Removes Minimap to Game UI
zox_sys2(MapInitializeSystem) {
    byte dbg_log = 0;
    int2 arrow_size = int2_single(8 * ui_scale);
    byte arrow_thickness = 0.5f * ui_scale;
    color arrow_fill = (color) { 33, 255, 177, 185 };
    color arrow_outline = (color) { 44, 166, 66, 233 };
    // float map_alpha = 0.2f;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InitializeEntity);
    zox_sys_in(MapZoom);
    zox_sys_in(Alpha);
    zox_sys_in(PlayerLink);
    zox_sys_in(TerrainLink);
    zox_sys_in(MapPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(InitializeEntity, state);
        zox_sys_i(MapZoom, zoom);
        zox_sys_i(Alpha, alpha);
        zox_sys_i(PlayerLink, player);
        zox_sys_i(TerrainLink, terrain);
        zox_sys_i(MapPosition, position);
        if (state->value != zox_dirty_active) {
            continue;
        }
        if (dbg_log) {
            zox_log("Map Initialized. Terrain [%s]. Center [%ix%i]. Zoom [%i].", zox_get_name(terrain->value), position->value.x, position->value.y, zoom->value);
        }
        entity body = zox_get_child_by_id(world, e, zox_id(WindowBody));
        if (!zox_valid(body)) {
            zox_loge("No body on Minimap");
            continue;
        }
        int2 size = zox_getv(body, LayoutSize);
        byte layer = zox_getv(body, Layer2D);
        // NOTE: For now just set from origin or something
        zox_geter(terrain->value, TunkLinks, tunks);
        int2 margin_size = int2_single(4 * ui_scale);
        int2 used_size = int2_subtract(size, int2_multiply1(margin_size, 2));
        int2 piece_size = int2_divide1(used_size, 1 + zoom->value * 2);
        for (int x = -zoom->value; x <= zoom->value; x++) {
            for (int y = -zoom->value; y <= zoom->value; y++) {
                int2 grid_position = (int2) { x, y };
                int2 tunk_position = int2_add(position->value, grid_position);
                entity tunk = int2_hashmap_get(tunks->value, tunk_position);
                // TODO: Calculate the map position here
                int2 piece_position = int2_multiply(grid_position, piece_size);
                // Spawn our map piece
                entity e2 = spawn_ui(world, prefab_element_shell, body, float2_half, piece_position, piece_size, int2_zero);
                zox_set_unique_name(e2, "map_piece");
                zox_set(e2, TunkLink, { tunk });
                zox_set(e2, TextureDirty, { zox_dirty_none });
                zox_set(e2, Generate, { zox_dirty_trigger });
                zox_set(e2, MapPiecePosition, { grid_position });
                if (dbg_log) {
                    zox_log("   - Piece [%ix%i], Tunk [%ix%i]: %s", grid_position.x, grid_position.y, tunk_position.x, tunk_position.y, zox_valid(tunk) ? "Valid" : "Invalid");
                }
                zox_set(e2, Layer2D, { layer + 1 });
                zox_set(e2, Alpha, { alpha->value });
                zox_add_tag(e2, TunkTexture);
                // zox_add_tag(e2, RegionTexture);
                // zox_add_tag(e2, TownTexture);
            }
        }
        // NOTE: Spawns a simple arrow for player direction
        entity e3 = spawn_uic(world, prefab_element_ready, body, float2_half, int2_zero, arrow_size, arrow_size, arrow_fill, arrow_outline);
        zox_set_unique_name(e3, "map_player");
        zox_set(e3, Layer2D, { layer + 2 });
        zox_set(e3, PlayerLink, { player->value });
        zox_add_tag(e3, MapArrow);
        // Texture
        zox_add_tag(e3, ArrowTexture);
        zox_set(e3, Generate, { zox_dirty_trigger });
        zox_set(e3, OutlineThickness, { arrow_thickness });
    }
} zox_sys_end(MapInitializeSystem);
