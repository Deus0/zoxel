// tst: stream_sides, walking about leaves no faces missing where lods meet (chunks3/sys/sides.c)
//  - a world of its own, a streamer stepping a chunk at a time along x then z, so chunks change lod as it goes
//  - once each step settles: every chunk's faces (each material's SidesOctree) built again from its voxels and its
//    neighbours as they are now, each at the depth it draws (chunk_side_depth), against the faces it holds, at the
//    depth it draws: one it should draw and doesn't is a hole; one it holds and shouldn't is hidden, counted apart
//  - its neighbour links are the chunks the terrain holds beside it
//  - was: faces worked out against a neighbour's voxels deeper than it drew, not rebuilt when its lod changed
//  - run: bash bsh/tst.sh stream_sides (medium terrain, tst.sh; zox_tst_seed=666 for the same land)
#define zox_tst_ss_steps 8
byte zox_tst_ss_stage = 0;
int zox_tst_ss_step = 0;
int zox_tst_ss_settled = 0;
int zox_tst_ss_frames = 0;
entity zox_tst_ss_streamer = 0;
int zox_tst_ss_missing = 0, zox_tst_ss_extra = 0, zox_tst_ss_chunks = 0, zox_tst_ss_lods = 0;
int zox_tst_ss_missing_chunks = 0;
int zox_tst_ss_stale_links = 0;    // a neighbour link that isn't the chunk the terrain holds there

// faces in fresh the held tree lacks (missing), and the other way (extra), at the depth the chunk draws (its mesh
//  reads its sides there, or at a closed node above it): faces at other depths don't show
static void zox_tst_ss_compare(const SidesOctree* fresh, const SidesOctree* held, byte depth, byte drawn,
    int* missing, int* extra, int* missing_directions)
{
    const SidesOctree* fresh_kids = fresh && depth < drawn && has_children_SidesOctree(fresh) ? get_children_SidesOctree(fresh) : NULL;
    const SidesOctree* held_kids = held && depth < drawn && has_children_SidesOctree(held) ? get_children_SidesOctree(held) : NULL;
    if (fresh_kids || held_kids) {
        for (byte i = 0; i < octree_length; i++) {
            zox_tst_ss_compare(fresh_kids ? &fresh_kids[i] : fresh, held_kids ? &held_kids[i] : held, depth + 1, drawn,
                missing, extra, missing_directions);
        }
        return;
    }
    const byte f = fresh ? fresh->value : 0, h = held ? held->value : 0;
    for (byte d = 0; d < 6; d++) {
        const byte lost = (f & (1 << d)) && !(h & (1 << d));
        *missing += lost;
        missing_directions[d] += lost;
        *extra += !(f & (1 << d)) && (h & (1 << d));
    }
}

// a chunk's faces built again as chunk_sides_system builds them, against those it holds; 1 if any are missing
static byte zox_tst_ss_check_chunk(ecs* world, entity chunk, byte* materials, entity* manager) {
    const entity chunk_manager = zox_get_link(world, chunk, BlockManagerLink);
    if (*manager != chunk_manager) {
        *manager = chunk_manager;
        get_block_materials(world, chunk_manager, materials);
    }
    const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
    const ChunkLinks* links = zox_get(zox_get_parent(world, chunk), ChunkLinks);
    const VoxelNode* neighbor_voxels[6];
    byte neighbor_depths[6];
    const byte depth = zox_getv(chunk, NodeDepth);
    const byte drawn = chunk_drawn_depth(world, chunk);
    for (byte j = 0; j < 6; j++) {
        const entity n = neighbors->value[j];
        const byte valid = zox_valid(n) && zox_has(n, NodeDepth) && zox_has(n, VoxelNode);
        neighbor_depths[j] = valid ? chunk_side_depth(world, n) : 0;
        neighbor_voxels[j] = valid ? zox_get(n, VoxelNode) : NULL;
        zox_tst_ss_lods += valid && neighbor_depths[j] != drawn && neighbor_depths[j] != chunk_side_open;
        const entity mapped = int3_hashmap_get(links->value, int3_add(zox_getv(chunk, ChunkPosition), get_direction_int3(j)));
        zox_tst_ss_stale_links += mapped != n && (zox_valid(mapped) || zox_valid(n));
    }
    const VoxelNode* voxels = zox_get(chunk, VoxelNode);
    entity chunk_materials[zox_block_materials];
    const uint count = get_chunk_materials(world, chunk, chunk_materials);
    int missing = 0, extra = 0;
    int missing_directions[6] = { 0 };
    for (uint j = 0; j < count; j++) {
        const entity material = chunk_materials[j];
        if (chunk_material_gpu(zox_getv(material, BlockMaterial))) {
            continue;
        }
        SidesOctree fresh;
        create_SidesOctree(&fresh);
        build_sides_dig(materials, zox_getv(material, BlockMaterial), voxels, neighbor_voxels, neighbor_depths, voxels,
            &fresh, depth, 0, byte3_zero, 0);
        zox_tst_ss_compare(&fresh, zox_get(material, SidesOctree), 0, drawn, &missing, &extra, missing_directions);
        close_SidesOctree(&fresh);
    }
    zox_tst_ss_missing += missing;
    zox_tst_ss_extra += extra;
    zox_tst_ss_chunks++;
    static int logged = 0;
    if (missing && logged++ < 12) {
        const int3 p = zox_getv(chunk, ChunkPosition);
        const int* m = missing_directions;
        zox_log("[tst]  chunk [%ix%ix%i] draws at [%i]: [%i] faces missing by direction [%i %i %i %i %i %i], its sides draw at [%i %i %i %i %i %i]",
            p.x, p.y, p.z, drawn, missing, m[0], m[1], m[2], m[3], m[4], m[5],
            neighbor_depths[0], neighbor_depths[1], neighbor_depths[2], neighbor_depths[3], neighbor_depths[4], neighbor_depths[5]);
    }
    return missing > 0;
}

static void zox_tst_ss_check(ecs* world, entity terrain) {
    byte materials[256];
    entity manager = 0;
    const int missing_before = zox_tst_ss_missing;
    int chunks_with = 0;
    iter it = zox_children(world, terrain);
    while (zox_children_next(it)) {
        for (int j = 0; j < it.count; j++) {
            const entity chunk = it.entities[j];
            if (zox_has(chunk, Chunk3) && zox_has(chunk, ChunkNeighbors) && zox_has(chunk, VoxelNode)) {
                chunks_with += zox_tst_ss_check_chunk(world, chunk, materials, &manager);
            }
        }
    }
    zox_tst_ss_missing_chunks += chunks_with;
    zox_log("[tst] step [%i]: [%i] faces missing in [%i] chunks", zox_tst_ss_step, zox_tst_ss_missing - missing_before, chunks_with);
}

byte zox_tst_stream_sides_ready(ecs* world) {
    const entity terrain = zox_tst_world_ready(world);
    if (!terrain) {
        return 0;
    }
    const float chunk = octree_size(zox_getv(terrain, NodeDepth)) * zox_getv(terrain, BlockScale);
    zox_tst_progress(zox_tst_ss_step * 10000 + zox_tst_ss_frames);
    zox_tst_ss_frames++;
    if (zox_tst_ss_stage == 0) {
        zox_tst_ss_streamer = spawn_streamer(world, prefab_streamer, terrain, (float3) { chunk * 0.5f, chunk, chunk * 0.5f });
        zox_setv(zox_tst_ss_streamer, StreamerLevel, 1);
        zox_tst_ss_stage = 1;
        return 0;
    }
    zox_tst_waiting = "the stream to settle";
    zox_tst_ss_settled = zox_tst_terrain_busy(world, terrain) ? 0 : zox_tst_ss_settled + 1;
    if (zox_tst_ss_frames < 10 || zox_tst_ss_settled < 3) {
        return 0;
    }
    zox_tst_ss_check(world, terrain);
    if (zox_tst_ss_step >= zox_tst_ss_steps) {
        return 1;
    }
    // a chunk along x, then along z
    float3 position = zox_getv(zox_tst_ss_streamer, Position3D);
    if (zox_tst_ss_step < zox_tst_ss_steps / 2) {
        position.x += chunk;
    } else {
        position.z += chunk;
    }
    zox_setv(zox_tst_ss_streamer, Position3D, position);
    zox_tst_ss_step++;
    zox_tst_ss_settled = 0;
    zox_tst_ss_frames = 0;
    return 0;
}

void zox_tst_stream_sides_check(ecs* world) {
    (void) world;
    zox_log("[tst] [metric] stream_sides [%i] chunk checks, [%i] sides beside another lod, faces missing [%i] in [%i], hidden ones held [%i]",
        zox_tst_ss_chunks, zox_tst_ss_lods, zox_tst_ss_missing, zox_tst_ss_missing_chunks, zox_tst_ss_extra);
    zox_tst_check(zox_tst_ss_step >= zox_tst_ss_steps, "the stream stopped at step [%i]", zox_tst_ss_step);
    zox_tst_check(zox_tst_ss_lods, "no chunk was beside another lod, nothing was checked");
    zox_tst_check(!zox_tst_ss_stale_links, "[%i] neighbour links aren't the chunk the terrain holds there", zox_tst_ss_stale_links);
    zox_tst_check(!zox_tst_ss_missing, "[%i] faces missing in [%i] chunks (a neighbour's lod changed, its side wasn't rebuilt)",
        zox_tst_ss_missing, zox_tst_ss_missing_chunks);
    // hidden faces held: a neighbour's lod changed and this side wasn't rebuilt (2859 before the fix, 5 after)
    zox_tst_check(zox_tst_ss_extra < 100, "[%i] faces held that a neighbour hides (its sides not rebuilt)", zox_tst_ss_extra);
}

void zox_tsts_stream_sides(ecs* world) {
    (void) world;
    zox_tst_add("stream_sides", zox_tst_stream_sides_ready, zox_tst_stream_sides_check, 180);
    zox_tst_set_setup("stream_sides", NULL, 1);
}
