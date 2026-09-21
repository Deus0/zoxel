// NOTE: Update Map Position from Player
zox_sys2(MapPositionSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(MapPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(MapPosition, position);
        entity player = zox_get_link(world, e, PlayerLink);
        if (!zox_valid(player)) {
            continue;
        }
        entity camera = zox_get_link(
            world,
            player,
            CameraLink);
        if (!zox_valid(camera) ||
            !zox_has(camera, StreamPosition2))
        {
            continue;
        }
        int2 new_position = zox_getv(camera, StreamPosition2);
        if (int2_equals(position->value, new_position)) {
            continue;
        }
        position->value = new_position;
        if (dbg_log) {
            zox_log("Map Position Updated [%ix%i]",
                new_position.x,
                new_position.y);
        }
        entity terrain = zox_get_link(world, e, TerrainLink);
        zox_geter(terrain, TunkLinks, tunks);
        // for all textures
        entity body = zox_get_child_by_id(
            world,
            e,
            zox_id(WindowBody));
        if (!zox_valid(body)) {
            zox_loge("No body on Minimap");
            continue;
        }
        iter it2 = zox_children(world, body);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                if (!zox_has(e2, MapPiecePosition)) {
                    continue;
                }
                // Unlink Old one
                entity old_tunk = zox_get_link(world, e2, TunkLink);
                if (old_tunk) {
                    zox_unlink(
                        world,
                        e2,
                        TunkLink,
                        old_tunk);
                }
                int2 grid_postion = zox_getv(e2, MapPiecePosition);
                int2 tunk_position = int2_add(
                    position->value,
                    grid_postion);
                entity tunk = int2_hashmap_get(
                    tunks->value,
                    tunk_position);
                if (!zox_valid(tunk)) {
                    zox_logw("MapPositionSystem: Tunk Not Found at [%ix%i]",
                        tunk_position.x,
                        tunk_position.y);
                } else {
                    zox_link(world, e2, TunkLink, tunk);
                }
                zox_setv(e2, GenerateTexture, zox_generate_texture_run);
                if (dbg_log >= 2) {
                    zox_log("Map [%s] Piece [%ix%i] Updated [%ix%i]",
                        zox_sys_e_name,
                        grid_postion.x,
                        grid_postion.y,
                        tunk_position.x,
                        tunk_position.y);
                }
            }
        }
    }
} zox_sys_end(MapPositionSystem);

