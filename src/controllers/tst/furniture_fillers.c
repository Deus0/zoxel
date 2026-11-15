// tst: furniture_fillers, a bed or door fills its box with the shared filler block (items/fun/multi_block.c)
//  - places (multi_block_place) a bed and a door in the player's chunk, and a bed at its edge whose other half is in
//    the next chunk
//  - their other voxels hold the filler, solid; placing into one is refused
//  - breaking the bed's filler takes its base too, breaking the door's base takes its top
//  - run: bash bsh/tst.sh furniture_fillers
byte zox_tst_ffill_stage = 0;
int zox_tst_ffill_frames = 0;
int3 zox_tst_ffill_bed, zox_tst_ffill_door, zox_tst_ffill_edge;
byte zox_tst_ffill_values[3];

static inline int zox_tst_ffill_at(ecs* world, entity terrain, int3 p, int x, int y, int z) {
    return terrain_voxel_at(world, terrain, (int3) { p.x + x, p.y + y, p.z + z });
}

byte zox_tst_furniture_fillers_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    zox_tst_ffill_frames++;
    zox_tst_progress(zox_tst_ffill_stage * 1000 + zox_tst_ffill_frames);
    if (zox_tst_ffill_stage == 0) {
        zox_tst_waiting = "air for a bed, a door and a bed across a chunk edge";
        const byte depth = zox_getv(terrain, NodeDepth);
        const int length = octree_size(depth);
        const int3 chunk = real_position_to_chunk_position(zox_getv(character, Position3D), length, zox_getv(terrain, BlockScale));
        const int3 origin = { chunk.x * length, chunk.y * length, chunk.z * length };
        for (int y = 0; y < length - 2; y++) {
            const int3 bed = { origin.x + 2, origin.y + y, origin.z + 8 };
            const int3 door = { origin.x + 6, origin.y + y, origin.z + 8 };
            const int3 edge = { origin.x + length - 1, origin.y + y, origin.z + 8 };
            if (zox_tst_ffill_at(world, terrain, bed, 0, 0, 0) || zox_tst_ffill_at(world, terrain, bed, 1, 0, 0) ||
                zox_tst_ffill_at(world, terrain, door, 0, 0, 0) || zox_tst_ffill_at(world, terrain, door, 0, 1, 0) ||
                zox_tst_ffill_at(world, terrain, edge, 0, 0, 0) || zox_tst_ffill_at(world, terrain, edge, 1, 0, 0)) {
                continue;
            }
            const entity blocks[2] = { zox_tst_fplace_block(world, realm, zox_id(BlockBed)), zox_tst_fplace_block(world, realm, zox_id(BlockDoor)) };
            zox_tst_ffill_values[0] = zox_getv(blocks[0], BlockIndex);
            zox_tst_ffill_values[1] = zox_getv(blocks[1], BlockIndex);
            zox_tst_ffill_values[2] = realm_filler_voxel(world, realm);
            zox_tst_check(zox_tst_ffill_values[2], "the realm has no filler block");
            zox_tst_check(multi_block_place(world, terrain, realm, bed, zox_tst_ffill_values[0]), "the bed wasn't placed");
            zox_tst_check(multi_block_place(world, terrain, realm, door, zox_tst_ffill_values[1]), "the door wasn't placed");
            zox_tst_check(multi_block_place(world, terrain, realm, edge, zox_tst_ffill_values[0]), "the bed across a chunk edge wasn't placed");
            zox_tst_ffill_bed = bed;
            zox_tst_ffill_door = door;
            zox_tst_ffill_edge = edge;
            zox_tst_ffill_stage = 1;
            zox_tst_ffill_frames = 0;
            return 0;
        }
        return zox_tst_ffill_frames > 600;
    }
    if (zox_tst_ffill_frames < 10) {
        return 0;
    }
    if (zox_tst_ffill_stage == 1) {
        const byte bed = zox_tst_ffill_values[0], door = zox_tst_ffill_values[1], filler = zox_tst_ffill_values[2];
        const int3 b = zox_tst_ffill_bed, d = zox_tst_ffill_door, e = zox_tst_ffill_edge;
        zox_log("[tst] bed [%i %i] door [%i %i] edge bed [%i %i], filler [%i]", zox_tst_ffill_at(world, terrain, b, 0, 0, 0),
            zox_tst_ffill_at(world, terrain, b, 1, 0, 0), zox_tst_ffill_at(world, terrain, d, 0, 0, 0), zox_tst_ffill_at(world, terrain, d, 0, 1, 0),
            zox_tst_ffill_at(world, terrain, e, 0, 0, 0), zox_tst_ffill_at(world, terrain, e, 1, 0, 0), filler);
        zox_tst_check(zox_tst_ffill_at(world, terrain, b, 0, 0, 0) == bed && zox_tst_ffill_at(world, terrain, b, 1, 0, 0) == filler,
            "the bed's voxels aren't the bed and the filler");
        zox_tst_check(zox_tst_ffill_at(world, terrain, d, 0, 0, 0) == door && zox_tst_ffill_at(world, terrain, d, 0, 1, 0) == filler,
            "the door's voxels aren't the door and the filler above");
        zox_tst_check(zox_tst_ffill_at(world, terrain, e, 0, 0, 0) == bed && zox_tst_ffill_at(world, terrain, e, 1, 0, 0) == filler,
            "the bed across the chunk edge isn't the bed and the filler in the next chunk");
        const entity filler_block = realm_voxel_block(world, realm, filler);
        zox_tst_check(zox_getv(filler_block, BlockCollider) == zox_block_solid, "the filler isn't solid");
        zox_tst_check(!multi_block_place(world, terrain, realm, (int3) { b.x + 1, b.y, b.z }, bed), "placing into the bed's filler wasn't refused");
        // break the bed's filler and the door's base
        terrain_queue_voxel(world, terrain, (int3) { b.x + 1, b.y, b.z }, 0);
        terrain_queue_voxel(world, terrain, d, 0);
        zox_tst_ffill_stage = 2;
        zox_tst_ffill_frames = 0;
        return 0;
    }
    if (zox_tst_ffill_frames < 20) {
        return 0;
    }
    const int3 b = zox_tst_ffill_bed, d = zox_tst_ffill_door;
    zox_tst_check(!zox_tst_ffill_at(world, terrain, b, 0, 0, 0) && !zox_tst_ffill_at(world, terrain, b, 1, 0, 0),
        "breaking the bed's filler left [%i %i]", zox_tst_ffill_at(world, terrain, b, 0, 0, 0), zox_tst_ffill_at(world, terrain, b, 1, 0, 0));
    zox_tst_check(!zox_tst_ffill_at(world, terrain, d, 0, 0, 0) && !zox_tst_ffill_at(world, terrain, d, 0, 1, 0),
        "breaking the door's base left [%i %i]", zox_tst_ffill_at(world, terrain, d, 0, 0, 0), zox_tst_ffill_at(world, terrain, d, 0, 1, 0));
    zox_tst_ffill_stage = 3;
    return 1;
}

void zox_tst_furniture_fillers_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_ffill_stage == 3, "it stopped at stage [%i] (no air found?)", zox_tst_ffill_stage);
}
