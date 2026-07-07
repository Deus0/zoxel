entity dbg_chunk;

extern entity get_linked_character(ecs*, entity);

void select_dbg_chunk(ecs* world) {

    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }

    entity character = get_linked_character(world, player);
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

    zox_set(dbg_chunk, BuildChunkMesh, { zox_dirty_trigger });
    zox_log("Triggered BuildChunkMesh [%s]", zox_get_name(dbg_chunk));
}

/*void key_down_dbg_chunk(ecs *world, int32_t keycode) {
    if (keycode == zox_key_f) {
        select_dbg_chunk(world);
    }
    if (keycode == zox_key_g) {
        refresh_dbg_chunk(world);
    }
    if (keycode == zox_key_h) {
        refresh_dbg_chunk2(world);
    }
}*/
