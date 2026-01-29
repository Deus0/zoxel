entity dbg_chunk;

void select_dbg_chunk(ecs* world) {

    if (!zox_valid(local_realm) || !zox_has(local_realm, GameLink)) {
        zox_log_error("no realm (local)");
        return;
    }

    zox_geter(local_realm, GameLink, game);
    if (!zox_valid(game->value)) {
        zox_log_error("realm has no game");
        return;
    }

    zox_geter(game->value, PlayerLinks, players);
    entity player = players->value[0];
    zox_geter_value(player, CharacterLink, entity, character);
    if (!zox_valid(character)) {
        return;
    }

    zox_geter(character, RaycastVoxelData, data);

    if (dbg_chunk != data->chunk) {
        dbg_chunk = data->chunk;
        if (zox_valid(dbg_chunk)) {
            zox_log("New Chunk Selected [%s]", zox_get_name(dbg_chunk));
        } else {
            zox_log("Chunk Deselected");
        }
    }
}

void refresh_dbg_chunk(ecs* world) {
    if (!zox_valid(dbg_chunk)) {
        return;
    }

    zox_set(dbg_chunk, VoxelNodeDirty, { zox_dirty_trigger });
        zox_log("Triggered VoxelNodeDirty [%s]", zox_get_name(dbg_chunk));
}


void refresh_dbg_chunk2(ecs* world) {
    if (!zox_valid(dbg_chunk)) {
        return;
    }

    zox_set(dbg_chunk, ChunkMeshDirty, { zox_dirty_trigger });
    zox_log("Triggered ChunkMeshDirty [%s]", zox_get_name(dbg_chunk));
}

void key_down_dbg_chunk(ecs *world, int32_t keycode) {
    if (keycode == SDLK_f) {
        select_dbg_chunk(world);
    }
    if (keycode == SDLK_g) {
        refresh_dbg_chunk(world);
    }
    if (keycode == SDLK_h) {
        refresh_dbg_chunk2(world);
    }
}