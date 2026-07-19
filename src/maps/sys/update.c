// NOTE: Update Map Position from Player
zox_sys2(MapPositionSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerLink);
    zox_sys_in(TerrainLink);
    zox_sys_out(MapPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerLink, player);
        zox_sys_i(TerrainLink, terrain);
        zox_sys_o(MapPosition, position);
        if (!zox_valid(player->value) || !zox_has(player->value, CameraLink)) {
            continue;
        }
        entity camera = zox_getv(player->value, CameraLink);
        if (!zox_valid(camera) || !zox_has(camera, StreamPosition2)) {
            continue;
        }
        int2 new_position = zox_getv(camera, StreamPosition2);
        if (int2_equals(position->value, new_position)) {
            continue;
        }
        position->value = new_position;
        if (dbg_log) {
            zox_log("Map Position Updated [%ix%i]", new_position.x, new_position.y);
        }
        zox_geter(terrain->value, TunkLinks, tunks);
        // for all textures
        entity body = zox_get_child_by_id(world, e, zox_id(WindowBody));
        if (!zox_valid(body)) {
            zox_loge("No body on Minimap");
            continue;
        }
        iter it2 = zox_children(world, body);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                if (!zox_valid(e2) || !zox_has(e2, MapPiecePosition)) {
                    continue;
                }
                int2 grid_postion = zox_getv(e2, MapPiecePosition);
                int2 tunk_position = int2_add(position->value, grid_postion);
                entity tunk = int2_hashmap_get(tunks->value, tunk_position);
                if (!zox_valid(tunk)) {
                    zox_logw("MapPositionSystem: Tunk Invalid at [%ix%i]", tunk_position.x, tunk_position.y);
                }
                zox_set(e2, TunkLink, { tunk });
                zox_set(e2, GenerateTexture, { zox_generate_texture_run });
                if (dbg_log >= 2) {
                    zox_log(" - Map Piece [%ix%i] Updated [%ix%i]", grid_postion.x, grid_postion.y, tunk_position.x, tunk_position.y);
                }
            }
        }
    }
} zox_sys_end(MapPositionSystem);

