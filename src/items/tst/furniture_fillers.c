// tst: furniture_fillers, a bed or door fills its box with a filler block (items/fun/multi_block.c): the shared one, a
//  door its own (it stops light while it's shut, block_filler_voxel)
//  - places (multi_block_place) a bed and a door in an all air chunk (air_chunk.c, no game, no player), and a bed at its edge whose other half is in
//    the next chunk
//  - their other voxels hold the filler, solid; placing into one is refused, and placing one over air
//  - breaking the bed's filler takes its base too, breaking the door's base takes its top, removing the ground under
//    the edge bed takes it
//  - run: bash bsh/tst.sh furniture_fillers
byte zox_tst_ffill_stage = 0;
int zox_tst_ffill_frames = 0;
int3 zox_tst_ffill_bed, zox_tst_ffill_door, zox_tst_ffill_edge;
byte zox_tst_ffill_values[3];

static inline int zox_tst_ffill_at(ecs* world, entity terrain, int3 p, int x, int y, int z) {
    return terrain_voxel_at(world, terrain, (int3) { p.x + x, p.y + y, p.z + z });
}

// a realm block that's solid ground (a cube with a collider, not a model or a filler)
static inline byte zox_tst_ground_voxel(ecs* world, entity realm) {
    const BlockLinks* blocks = zox_get(realm, BlockLinks);
    for (int i = 0; i < blocks->length; i++) {
        const entity b = blocks->value[i];
        if (zox_valid(b) && !zox_has(b, BlockVox) && !zox_has(b, BlockFiller) &&
            zox_has(b, BlockCollider) && zox_getv(b, BlockCollider) == zox_block_solid) {
            return i + 1;
        }
    }
    return 0;
}

// places a sized block with ground written under its bottom cells first (straight into the octree, so placing sees it)
static inline byte zox_tst_place_supported(ecs* world, entity terrain, entity realm, int3 base, byte value, byte direction) {
    const int3 size = block_size_of(world, realm_voxel_block(world, realm, value));
    const byte ground = zox_tst_ground_voxel(world, realm);
    for (int x = 0; x < size.x; x++)
    for (int z = 0; z < size.z; z++) {
        const int3 o = block_direction_offset(direction, (int3) { x, 0, z });
        byte3 local;
        const entity chunk = terrain_chunk_local(world, terrain, (int3) { base.x + o.x, base.y + o.y - 1, base.z + o.z }, &local);
        if (!chunk) {
            continue;
        }
        spinlock* lock = zox_octree_lock(world, chunk);
        zox_octree_lock_begin(lock);
        set_VoxelNode(zox_getm(chunk, VoxelNode), zox_getv(chunk, NodeDepth), local, ground);
        zox_octree_lock_end(lock);
        zox_add(chunk, VoxelNodeDirty);
    }
    return multi_block_place(world, terrain, realm, base, value, direction);
}

byte zox_tst_furniture_fillers_ready(ecs* world) {
    // an all air chunk (terrain/tst/air_chunk.c): no game, no player
    const entity air = zox_tst_streamed_air_chunk(world);
    if (!air) {
        return 0;
    }
    const entity terrain = zox_tst_world_terrain;
    const entity realm = zox_tst_world_realm;
    zox_tst_ffill_frames++;
    zox_tst_progress(zox_tst_ffill_stage * 1000 + zox_tst_ffill_frames);
    if (zox_tst_ffill_stage == 0) {
        zox_tst_waiting = "air for a bed, a door and a bed across a chunk edge";
        const byte depth = zox_getv(terrain, NodeDepth);
        const int length = octree_size(depth);
        const int3 chunk = zox_getv(air, ChunkPosition);
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
            zox_tst_check(zox_tst_place_supported(world, terrain, realm, bed, zox_tst_ffill_values[0], 0), "the bed wasn't placed");
            zox_tst_check(zox_tst_place_supported(world, terrain, realm, door, zox_tst_ffill_values[1], 0), "the door wasn't placed");
            zox_tst_check(zox_tst_place_supported(world, terrain, realm, edge, zox_tst_ffill_values[0], 0), "the bed across a chunk edge wasn't placed");
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
        zox_tst_check(zox_tst_ffill_at(world, terrain, d, 0, 0, 0) == door && zox_tst_ffill_at(world, terrain, d, 0, 1, 0) ==
            block_filler_voxel(world, realm, realm_voxel_block(world, realm, door)),
            "the door's voxels aren't the door and the filler above");
        zox_tst_check(zox_tst_ffill_at(world, terrain, e, 0, 0, 0) == bed && zox_tst_ffill_at(world, terrain, e, 1, 0, 0) == filler,
            "the bed across the chunk edge isn't the bed and the filler in the next chunk");
        const entity filler_block = realm_voxel_block(world, realm, filler);
        zox_tst_check(zox_getv(filler_block, BlockCollider) == zox_block_solid, "the filler isn't solid");
        zox_tst_check(!multi_block_place(world, terrain, realm, (int3) { b.x + 1, b.y, b.z }, bed, 0), "placing into the bed's filler wasn't refused");
        // over air: a bed two blocks above the door's top, nothing under it
        zox_tst_check(!multi_block_place(world, terrain, realm, (int3) { d.x, d.y + 4, d.z }, bed, 0), "a bed over air wasn't refused");
        // break the bed's filler and the door's base
        terrain_queue_voxel(world, terrain, (int3) { b.x + 1, b.y, b.z }, 0, 0);
        terrain_queue_voxel(world, terrain, d, 0, 0);
        // and the ground under the edge bed's other half (in the next chunk)
        terrain_queue_voxel(world, terrain, (int3) { e.x + 1, e.y - 1, e.z }, 0, 0);
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
    const int3 e = zox_tst_ffill_edge;
    zox_tst_check(!zox_tst_ffill_at(world, terrain, e, 0, 0, 0) && !zox_tst_ffill_at(world, terrain, e, 1, 0, 0),
        "removing the ground under the edge bed left [%i %i]", zox_tst_ffill_at(world, terrain, e, 0, 0, 0), zox_tst_ffill_at(world, terrain, e, 1, 0, 0));
    zox_tst_ffill_stage = 3;
    return 1;
}

void zox_tst_furniture_fillers_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_ffill_stage == 3, "it stopped at stage [%i] (no air found?)", zox_tst_ffill_stage);
}

void zox_tsts_furniture_fillers(ecs* world) {
    zox_tst_add("furniture_fillers", zox_tst_furniture_fillers_ready, zox_tst_furniture_fillers_check, 120);
    zox_tst_set_setup("furniture_fillers", NULL, 1);
}
