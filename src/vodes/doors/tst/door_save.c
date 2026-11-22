// tst: door_save, a shut room saved and loaded stays dark: no light past its shut door (vodes/doors, chunks3/io)
//  - an all air chunk in the void (terrain tst air_chunk.c, no player), sunlit: a sealed brick room (inside x 2 to 7,
//    y 4 to 6, z 6 to 8), a shut door in its +z wall at x 4 (its base and door filler), the sun outside
//  - built: inside dark; opened and shut (door_toggle); saved (Edited, chunk3_save_system to its realm's folder), its block data and light wiped,
//    loaded from the file as chunk3_load_system does (Loaded, its light from the sunlight step): inside dark, the door
//    shut in its block data, outside sunlit
//  - run: bash bsh/tst.sh door_save
#define zox_tst_dsave_door_x 4
byte zox_tst_dsave_stage = 0;
int zox_tst_dsave_settled = 0;
int zox_tst_dsave_frames = 0;
byte zox_tst_dsave_inside[2];   // built, loaded: the brightest air inside
byte3 zox_tst_dsave_at[2];
byte zox_tst_dsave_outside[2];  // beside the door outside
byte zox_tst_dsave_data[2][2];  // the door's base and filler block data

static inline byte zox_tst_dsave_room(int x, int y, int z) {
    return x >= 1 && x <= 8 && y >= 3 && y <= 7 && z >= 5 && z <= 9;
}

static inline byte zox_tst_dsave_interior(int x, int y, int z) {
    return x >= 2 && x <= 7 && y >= 4 && y <= 6 && z >= 6 && z <= 8;
}

static inline void zox_tst_dsave_read(ecs* world, entity chunk, byte s) {
    const byte depth = zox_getv(chunk, NodeDepth);
    const LightNode* lights = zox_get(chunk, LightNode);
    zox_tst_dsave_inside[s] = 0;
    for (byte x = 2; x <= 7; x++) for (byte y = 4; y <= 6; y++) for (byte z = 6; z <= 8; z++) {
        const byte l = getv_LightNode(lights, depth, (byte3) { x, y, z });
        if (l > zox_tst_dsave_inside[s]) {
            zox_tst_dsave_inside[s] = l;
            zox_tst_dsave_at[s] = (byte3) { x, y, z };
        }
    }
    zox_tst_dsave_outside[s] = getv_LightNode(lights, depth, (byte3) { zox_tst_dsave_door_x, 4, 10 });
    const BlockDatas* datas = zox_get(chunk, BlockDatas);
    zox_tst_dsave_data[s][0] = get_block_data(datas, (byte3) { zox_tst_dsave_door_x, 4, 9 });
    zox_tst_dsave_data[s][1] = get_block_data(datas, (byte3) { zox_tst_dsave_door_x, 5, 9 });
}

static inline char* zox_tst_dsave_path(ecs* world, entity chunk) {
    char filename[128];
    get_chunk_filename(filename, zox_getv(chunk, ChunkPosition));
    return join_path(zox_get(zox_tst_world_realm, FolderPath)->value, filename);
}

byte zox_tst_door_save_ready(ecs* world) {
    const entity chunk = zox_tst_streamed_air_chunk(world);
    if (!chunk) {
        return 0;
    }
    zox_tst_dsave_frames++;
    zox_tst_progress(zox_tst_dsave_stage * 100 + zox_tst_dsave_settled);
    const entity realm = zox_tst_world_realm;
    if (zox_tst_dsave_stage == 0) {
        // no old file from a run before
        char* path = zox_tst_dsave_path(world, chunk);
        remove(path);
        free(path);
        const byte bricks = zox_getv(zox_tst_air_block(world, zox_id(BlockBricks)), BlockIndex);
        const entity door = zox_tst_air_block(world, zox_id(BlockDoor));
        for (byte x = 1; x <= 8; x++) for (byte y = 3; y <= 7; y++) for (byte z = 5; z <= 9; z++) {
            if (zox_tst_dsave_interior(x, y, z)) {
                continue;
            }
            if (x == zox_tst_dsave_door_x && z == 9 && y == 4) {
                zox_tst_room_set(world, chunk, (byte3) { x, y, z }, zox_getv(door, BlockIndex), 1);
            } else if (x == zox_tst_dsave_door_x && z == 9 && y == 5) {
                zox_tst_room_set(world, chunk, (byte3) { x, y, z }, block_filler_voxel(world, realm, door),
                    block_offset_encode((int3) { 0, -1, 0 }));
            } else {
                zox_tst_room_set(world, chunk, (byte3) { x, y, z }, bricks, 0);
            }
        }
        zox_tst_dsave_stage = 1;
        return 0;
    }
    if (zox_tst_dsave_stage == 3 && zox_has(chunk, GenerateChunk) && zox_getv(chunk, GenerateChunk)) {
        zox_tst_waiting = "the loaded chunk's light to start";
        return 0;
    }
    if (zox_tst_dsave_stage != 2 && !zox_tst_room_settled(world, chunk, &zox_tst_dsave_settled)) {
        zox_tst_waiting = zox_tst_dsave_stage == 1 ? "the room built" : "the loaded room's light";
        return 0;
    }
    if (zox_tst_dsave_stage == 1) {
        zox_tst_dsave_read(world, chunk, 0);
        // opened and shut again, as a player does (door_toggle), before it's saved
        const int3 c = zox_getv(chunk, ChunkPosition);
        const int length = octree_size(zox_getv(chunk, NodeDepth));
        const int3 base = { c.x * length + zox_tst_dsave_door_x, c.y * length + 4, c.z * length + 9 };
        zox_tst_check(door_toggle(world, zox_tst_world_terrain, realm, base), "opening the door did nothing");
        zox_tst_check(door_toggle(world, zox_tst_world_terrain, realm, base), "shutting the door did nothing");
        zox_tst_dsave_stage = 2;
        zox_tst_dsave_frames = 0;
        return 0;
    }
    if (zox_tst_dsave_stage == 2) {
        // saved: its edits marked it Edited, chunk3_save_system writes it (a few frames, the file there)
        zox_tst_waiting = "the chunk saved";
        char* path = zox_tst_dsave_path(world, chunk);
        const byte saved = file_exists(path);
        free(path);
        if (!saved || zox_tst_dsave_frames < 10) {
            return zox_tst_dsave_frames > 600;
        }
        // its block data and light wiped, loaded from the file over it, as chunk3_load_system
        spinlock* lock = zox_octree_lock(world, chunk);
        zox_octree_lock_begin(lock);
        BlockDatas* datas = zox_getm(chunk, BlockDatas);
        for (int i = 0; i < datas->length; i++) {
            datas->value[i] = 255;
        }
        const byte loaded = load_chunk(world, realm, zox_getv(chunk, ChunkPosition), zox_getm(chunk, VoxelNode), datas);
        zox_octree_lock_end(lock);
        zox_tst_check(loaded, "the chunk didn't load from its file");
        spinlock* light_lock = &zox_getm(chunk, LightLock)->value;
        spin_lock(light_lock);
        LightNode* lights = zox_getm(chunk, LightNode);
        collapse_LightNode(lights);
        lights->value = darklight;
        spin_unlock(light_lock);
        zox_add(chunk, Loaded);
        zox_setv(chunk, GenerateChunk, zox_generate_terrain_sunlight);
        zox_add(chunk, VoxelNodeDirty);
        zox_tst_dsave_settled = 0;
        zox_tst_dsave_stage = 3;
        return 0;
    }
    zox_tst_dsave_read(world, chunk, 1);
    zox_tst_dsave_stage = 4;
    return 1;
}

void zox_tst_door_save_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_dsave_stage == 4, "never got to the end, at stage [%i]", zox_tst_dsave_stage);
    const char* names[2] = { "built", "loaded" };
    for (byte s = 0; s < 2; s++) {
        const byte3 a = zox_tst_dsave_at[s];
        zox_log("[tst] %s: inside brightest [%i] at [%ix%ix%i], outside [%i], door data base [%i] filler [%i]", names[s],
            zox_tst_dsave_inside[s], a.x, a.y, a.z, zox_tst_dsave_outside[s], zox_tst_dsave_data[s][0], zox_tst_dsave_data[s][1]);
        zox_tst_check(zox_tst_dsave_outside[s] > darklight + 100, "%s: outside the door isn't lit [%i]", names[s], zox_tst_dsave_outside[s]);
        zox_tst_check(!(zox_tst_dsave_data[s][0] & block_data_open) && !(zox_tst_dsave_data[s][1] & block_data_open),
            "%s: the door's open, data base [%i] filler [%i]", names[s], zox_tst_dsave_data[s][0], zox_tst_dsave_data[s][1]);
        zox_tst_check(zox_tst_dsave_inside[s] <= darklight, "%s: light inside the shut room [%i]", names[s], zox_tst_dsave_inside[s]);
    }
}
