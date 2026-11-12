// tst: block_vox_lights, minivoxes (block voxes) are lit like the terrain around them, and relit when it changes
//  - waits for minivoxes in chunks whose lights have settled (no LightNodeDirty on them or their neighbors)
//  - lit: every settled minivox's Brightness matches the light around it (get_block_vox_brightness),
//    and some are bright (sunlit grass isn't left at darklight)
//  - relit: a chunk's lights are set to darklight and marked dirty, then
//      - its minivoxes match their new light, most of the inner ones dark (block_vox_lights_system)
//        (the terrain's sunlight pass can refill the chunk, i.e. after a lod change, then it's darkened again)
//      - its own terrain meshes rebuild darker colors (chunk_colors_trigger_system used to skip them)
//  - run: bash bsh/tst.sh block_vox_lights

#define zox_tst_bvl_settle_seconds 3.0
#define zox_tst_bvl_relight_seconds 4.0
#define zox_tst_bvl_min_voxes 8

byte zox_tst_bvl_stage = 0;
double zox_tst_bvl_time = 0;
entity zox_tst_bvl_chunk = 0;
float zox_tst_bvl_colors_before = 0;
int zox_tst_bvl_voxes = 0;          // settled minivoxes checked
int zox_tst_bvl_stale = 0;          // their Brightness didn't match their light
int zox_tst_bvl_bright = 0;         // lit above darklight
int zox_tst_bvl_relit_dark = 0;     // inner minivoxes of the darkened chunk that went dark
int zox_tst_bvl_relit_total = 0;
int zox_tst_bvl_relit_stale = 0;    // their Brightness didn't match their light after relighting
int zox_tst_bvl_darkens = 0;        // times the chunk was darkened (the lights refilled it before)
#define zox_tst_bvl_max_darkens 5

// sets chunk's lights to darklight and marks them changed, like the light going out
static inline void zox_tst_bvl_darken(ecs* world, entity chunk) {
    LightNode* lights = zox_getm(chunk, LightNode);
    collapse_LightNode(lights);
    lights->value = darklight;
    zox_setv(chunk, LightNodeDirty, zox_dirty_trigger);
    zox_tst_bvl_darkens++;
    zox_tst_bvl_time = zox_current_time;
}
float zox_tst_bvl_colors_after = 0;

static inline byte zox_tst_bvl_is_block_vox(ecs* world, entity e) {
    return zox_has(e, VoxelLocalPosition) && zox_has(e, Brightness);
}

// lights of chunk and its neighbors have settled
static inline byte zox_tst_bvl_chunk_settled(ecs* world, entity chunk) {
    if (!zox_has(chunk, LightNode) || !zox_has(chunk, LightNodeDirty) || zox_getv(chunk, LightNodeDirty)) {
        return 0;
    }
    zox_geter(chunk, ChunkNeighbors, neighbors);
    for (byte j = 0; j < chunk_neighbors_length; j++) {
        const entity n = neighbors->value[j];
        if (zox_valid(n) && zox_has(n, LightNodeDirty) && zox_getv(n, LightNodeDirty)) {
            return 0;
        }
    }
    return 1;
}

// average light color of chunk's terrain meshes
static inline float zox_tst_bvl_mesh_colors(ecs* world, entity chunk) {
    double total = 0;
    int count = 0;
    iter it2 = zox_children(world, chunk);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity mesh = it2.entities[j];
            if (!zox_has(mesh, ChunkMesh) || !zox_has(mesh, MeshColorRGBs)) {
                continue;
            }
            zox_geter(mesh, MeshColorRGBs, colors);
            for (int k = 0; k < colors->length; k++) {
                total += colors->value[k].r;
                count++;
            }
        }
    }
    return count ? (float) (total / count) : 0;
}

// counts settled minivoxes, and the ones whose Brightness is stale
static inline void zox_tst_bvl_scan(ecs* world, entity* best_chunk) {
    zox_tst_bvl_voxes = 0;
    zox_tst_bvl_stale = 0;
    zox_tst_bvl_bright = 0;
    int best_count = 0;
    const float dark = block_vox_light_to_brightness(darklight);
    iter it2 = zox_children(world, local_terrain);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity chunk = it2.entities[j];
            if (!zox_has(chunk, Chunk3) || !zox_tst_bvl_chunk_settled(world, chunk)) {
                continue;
            }
            int count = 0;
            iter it3 = zox_children(world, chunk);
            while (zox_children_next(it3)) {
                for (int k = 0; k < it3.count; k++) {
                    const entity e = it3.entities[k];
                    if (!zox_tst_bvl_is_block_vox(world, e)) {
                        continue;
                    }
                    const float expected = get_block_vox_brightness(world, chunk, zox_getv(e, VoxelLocalPosition));
                    const float brightness = zox_getv(e, Brightness);
                    zox_tst_bvl_voxes++;
                    zox_tst_bvl_stale += fabsf(brightness - expected) > 0.001f;
                    zox_tst_bvl_bright += brightness > dark + 0.01f;
                    count++;
                }
            }
            // darkened later, so it needs built, lit meshes (offscreen chunks may not have built theirs yet)
            if (count > best_count && zox_tst_bvl_mesh_colors(world, chunk) > darklight * 1.5f) {
                best_count = count;
                *best_chunk = chunk;
            }
        }
    }
}

// minivoxes not on the chunk's border, their light only comes from this chunk
static inline void zox_tst_bvl_count_relit(ecs* world, entity chunk) {
    zox_tst_bvl_relit_dark = 0;
    zox_tst_bvl_relit_total = 0;
    zox_tst_bvl_relit_stale = 0;
    const int length = octree_size(zox_getv(chunk, NodeDepth));
    const float dark = block_vox_light_to_brightness(darklight);
    iter it2 = zox_children(world, chunk);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity e = it2.entities[j];
            if (!zox_tst_bvl_is_block_vox(world, e)) {
                continue;
            }
            const byte3 p = zox_getv(e, VoxelLocalPosition);
            const float expected = get_block_vox_brightness(world, chunk, p);
            zox_tst_bvl_relit_stale += fabsf(zox_getv(e, Brightness) - expected) > 0.001f;
            if (p.x == 0 || p.y == 0 || p.z == 0 || p.x == length - 1 || p.y == length - 1 || p.z == length - 1) {
                continue;
            }
            zox_tst_bvl_relit_total++;
            zox_tst_bvl_relit_dark += fabsf(zox_getv(e, Brightness) - dark) < 0.001f;
        }
    }
}

byte zox_tst_block_vox_lights_ready(ecs* world) {
    if (!zox_valid(local_terrain)) {
        zox_tst_waiting = "the terrain";
        return 0;
    }
    if (zox_tst_bvl_stage == 0) {
        // enough settled minivoxes, all up to date, for a few seconds
        entity chunk = 0;
        zox_tst_bvl_scan(world, &chunk);
        zox_tst_progress(zox_tst_bvl_voxes);
        static char waiting[160];
        snprintf(waiting, sizeof(waiting), "settled minivoxes with up to date brightness: [%i] minivoxes, [%i] stale",
            zox_tst_bvl_voxes, zox_tst_bvl_stale);
        zox_tst_waiting = waiting;
        if (zox_tst_bvl_voxes < zox_tst_bvl_min_voxes || zox_tst_bvl_stale || !chunk) {
            zox_tst_bvl_time = zox_current_time;
            return 0;
        }
        if (zox_current_time - zox_tst_bvl_time < zox_tst_bvl_settle_seconds) {
            return 0;
        }
        zox_log("[tst] lit: [%i] settled minivoxes, [%i] stale, [%i] brighter than darklight",
            zox_tst_bvl_voxes, zox_tst_bvl_stale, zox_tst_bvl_bright);
        // darken the chunk with the most minivoxes, like the light going out
        zox_tst_bvl_chunk = chunk;
        zox_tst_bvl_colors_before = zox_tst_bvl_mesh_colors(world, chunk);
        zox_tst_bvl_darken(world, chunk);
        zox_log("[tst] darkened [%s], its mesh colors average [%.1f]", zox_getn(chunk), zox_tst_bvl_colors_before);
        zox_tst_bvl_stage = 1;
        return 0;
    }
    zox_tst_waiting = "the darkened chunk's minivoxes and meshes to relight";
    zox_tst_progress(zox_current_time - zox_tst_bvl_time);
    if (!zox_valid(zox_tst_bvl_chunk)) {
        return 1;
    }
    zox_tst_bvl_count_relit(world, zox_tst_bvl_chunk);
    zox_tst_bvl_colors_after = zox_tst_bvl_mesh_colors(world, zox_tst_bvl_chunk);
    const byte minivoxes_dark = !zox_tst_bvl_relit_stale && zox_tst_bvl_relit_dark * 2 > zox_tst_bvl_relit_total;
    const byte meshes_darker = zox_tst_bvl_colors_after < zox_tst_bvl_colors_before * 0.75f;
    // minivoxes follow their light (not stale) but it's bright again: the lights refilled the chunk, darken it again
    //  - stale minivoxes mean the relight hasn't run yet
    if (!zox_tst_bvl_relit_stale && !minivoxes_dark && zox_tst_bvl_relit_total &&
        zox_tst_bvl_darkens < zox_tst_bvl_max_darkens)
    {
        zox_log("[tst] the lights refilled [%s], darkening it again", zox_getn(zox_tst_bvl_chunk));
        zox_tst_bvl_darken(world, zox_tst_bvl_chunk);
        return 0;
    }
    return (minivoxes_dark && meshes_darker) ||
        zox_current_time - zox_tst_bvl_time > zox_tst_bvl_relight_seconds;
}

void zox_tst_block_vox_lights_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_bvl_voxes >= zox_tst_bvl_min_voxes, "only [%i] settled minivoxes", zox_tst_bvl_voxes);
    zox_tst_check(!zox_tst_bvl_stale, "[%i] of [%i] minivoxes have a stale Brightness", zox_tst_bvl_stale, zox_tst_bvl_voxes);
    zox_tst_check(zox_tst_bvl_bright, "none of [%i] minivoxes are brighter than darklight", zox_tst_bvl_voxes);
    zox_tst_check(zox_valid(zox_tst_bvl_chunk), "the darkened chunk is gone");
    zox_log("[tst] relit: [%i/%i] inner minivoxes dark, mesh colors [%.1f] -> [%.1f]",
        zox_tst_bvl_relit_dark, zox_tst_bvl_relit_total, zox_tst_bvl_colors_before, zox_tst_bvl_colors_after);
    zox_tst_check(!zox_tst_bvl_relit_stale,
        "[%i] of the darkened chunk's minivoxes have a stale Brightness", zox_tst_bvl_relit_stale);
    zox_tst_check(zox_tst_bvl_relit_total && zox_tst_bvl_relit_dark * 2 > zox_tst_bvl_relit_total,
        "only [%i] of [%i] inner minivoxes went dark after their chunk's lights did", zox_tst_bvl_relit_dark, zox_tst_bvl_relit_total);
    zox_tst_check(zox_tst_bvl_colors_before > darklight * 1.5f,
        "the darkened chunk's meshes were already dark [%.1f], pick a lit chunk", zox_tst_bvl_colors_before);
    zox_tst_check(zox_tst_bvl_colors_after < zox_tst_bvl_colors_before * 0.75f,
        "the darkened chunk's meshes didn't rebuild their light colors [%.1f] -> [%.1f]",
        zox_tst_bvl_colors_before, zox_tst_bvl_colors_after);
}

// tst: block_vox_light_sampling, the light a minivox takes (get_block_vox_light), no game needed
//  - the brightest of its own voxel and its 6 sides, never a diagonal or further voxel
//  - on a chunk edge, the side past it comes from the neighbor chunk, none = skipped
byte zox_tst_bvl_sampling_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_bvl_sampling_check(ecs* world) {
    (void) world;
    const byte depth = 3;
    const byte length = octree_size(depth);
    LightNode chunk;
    create_LightNode(&chunk);
    chunk.value = darklight;
    LightNode neighbor;
    create_LightNode(&neighbor);
    neighbor.value = darklight;
    const LightNode* none[6] = { 0 };
    const LightNode* neighbors[6] = { 0 };
    neighbors[direction_right] = &neighbor;
    // dark everywhere
    const byte3 middle = { 4, 4, 4 };
    zox_tst_check(get_block_vox_light(&chunk, none, depth, middle) == darklight, "dark chunk didn't give darklight");
    // one lit side wins, from each direction
    for (byte d = 0; d < 6; d++) {
        int3 side = move_position(byte3_to_int3(middle), d);
        set_LightNode(&chunk, depth, int3_to_byte3(side), 200);
        const byte light = get_block_vox_light(&chunk, none, depth, middle);
        zox_tst_check(light == 200, "lit side [%i] gave [%i], expected [200]", d, light);
        set_LightNode(&chunk, depth, int3_to_byte3(side), darklight);
    }
    // a diagonal doesn't count
    set_LightNode(&chunk, depth, (byte3) { 5, 5, 4 }, 200);
    zox_tst_check(get_block_vox_light(&chunk, none, depth, middle) == darklight, "a diagonal voxel lit the minivox");
    // its own voxel counts, i.e. light passing blocks later
    set_LightNode(&chunk, depth, middle, 120);
    zox_tst_check(get_block_vox_light(&chunk, none, depth, middle) == 120, "its own voxel's light was ignored");
    // on the +x edge: the neighbor's touching voxel lights it, its far voxel doesn't
    const byte3 edge = { length - 1, 2, 2 };
    set_LightNode(&neighbor, depth, (byte3) { length - 1, 2, 2 }, 250);
    zox_tst_check(get_block_vox_light(&chunk, neighbors, depth, edge) == darklight, "the neighbor's far voxel lit an edge minivox");
    set_LightNode(&neighbor, depth, (byte3) { 0, 2, 2 }, 230);
    zox_tst_check(get_block_vox_light(&chunk, neighbors, depth, edge) == 230, "the neighbor's touching voxel didn't light an edge minivox");
    // no neighbor chunk: only its own chunk
    zox_tst_check(get_block_vox_light(&chunk, none, depth, edge) == darklight, "an edge minivox without a neighbor chunk wasn't darklight");
    // brightness is light / 255, like terrain vertex colors
    zox_tst_check(fabsf(block_vox_light_to_brightness(255) - 1.0f) < 0.0001f, "full light isn't brightness 1");
    collapse_LightNode(&neighbor);
    collapse_LightNode(&chunk);
}

void zox_tsts_vodes3() {
    zox_tst_add("block_vox_light_sampling", zox_tst_bvl_sampling_ready, zox_tst_bvl_sampling_check, 30);
    zox_tst_add("block_vox_lights", zox_tst_block_vox_lights_ready, zox_tst_block_vox_lights_check, 90);
}
