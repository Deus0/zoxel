// tst: map_pixel, the minimap moves per texel (a block), not per tunk, and is quick to draw (maps/sys/view.c)
//  - holds the player's character still (high up) while the land round it streams in
//  - checks: moving a third of a block doesn't redraw it; moving a block east redraws it a texel over, the land the
//      same shifted by one texel; a chunk changing (VoxelNodeDirty) clears its tunk's colours and redraws it
//  - logs: how long a whole redraw takes
//  - run: bash bsh/tst.sh map_pixel
#define zox_tst_mp_texels_max 512
int zox_tst_mp_stage = 0;
int zox_tst_mp_frames = 0;
float3 zox_tst_mp_at;
entity zox_tst_mp_view = 0;
int2 zox_tst_mp_drawn;
color zox_tst_mp_before[zox_tst_mp_texels_max * zox_tst_mp_texels_max];
int zox_tst_mp_texels = 0;
byte zox_tst_mp_still = 0, zox_tst_mp_moved = 0, zox_tst_mp_redrawn = 0, zox_tst_mp_cleared = 0;
int zox_tst_mp_compared = 0, zox_tst_mp_mismatched = 0, zox_tst_mp_version = 0;
double zox_tst_mp_draw_ms = -1;

// its terrain's map version (set once a tunk's colours are first cleared)
static inline int zox_tst_mp_maps_version(ecs* world, entity terrain) {
    return zox_has(terrain, TunkMapsVersion) ? zox_getv(terrain, TunkMapsVersion) : 0;
}

// the minimap's view: canvas > minimap > body > view
static inline entity zox_tst_mp_find_view(ecs* world, entity e, byte depth) {
    if (zox_has(e, MapView)) {
        return e;
    }
    entity found = 0;
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count && !found && depth < 4; i++) {
            found = zox_tst_mp_find_view(world, it.entities[i], depth + 1);
        }
    }
    return found;
}

byte zox_tst_map_pixel_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const float scale = zox_getv(terrain, BlockScale);
    zox_tst_mp_frames++;
    zox_tst_progress(zox_tst_mp_stage);
    if (zox_tst_mp_stage == 0) {
        zox_tst_mp_view = zox_tst_mp_find_view(world, zox_get_link(world, dbg_player, CanvasLink), 0);
        if (!zox_valid(zox_tst_mp_view)) {
            zox_tst_waiting = "the minimap's view";
            return 0;
        }
        // the middle of a block, up high
        const float3 p = zox_getv(character, Position3D);
        zox_tst_mp_at = (float3) { (floorf(p.x / scale) + 0.5f) * scale, 90 * scale, (floorf(p.z / scale) + 0.5f) * scale };
        zox_tst_mp_stage = 1;
        zox_tst_mp_frames = 0;
    }
    zox_setv(character, Position3D, zox_tst_mp_at);
    zox_setv(character, Velocity3D, float3_zero);
    const entity view = zox_tst_mp_view;
    if (zox_tst_mp_stage == 1) {
        zox_tst_waiting = "the minimap drawn and settled";
        if (zox_tst_mp_frames < 90) {
            return 0;   // the land round it streamed in, its tunks made
        }
        const TextureData* data = zox_get(view, TextureData);
        zox_tst_mp_texels = zox_getv(view, TextureSize).x;
        if (zox_tst_mp_texels > zox_tst_mp_texels_max || data->length != zox_tst_mp_texels * zox_tst_mp_texels) {
            zox_tst_check(0, "the minimap's view is [%i] texels, its data [%i]", zox_tst_mp_texels, data->length);
            return 1;
        }
        memcpy(zox_tst_mp_before, data->value, data->length * sizeof(color));
        zox_tst_mp_drawn = zox_getv(view, MapDrawnAt);
        // a third of a block east: the same texel
        zox_tst_mp_at.x += scale / 3;
        zox_tst_mp_stage = 2;
        zox_tst_mp_frames = 0;
        return 0;
    }
    if (zox_tst_mp_stage == 2) {
        if (zox_tst_mp_frames < 5) {
            return 0;
        }
        zox_tst_mp_still = int2_equals(zox_getv(view, MapDrawnAt), zox_tst_mp_drawn);
        // a block east of where it was drawn: a texel over (a texel a block on the minimap)
        zox_tst_mp_at.x += scale * zox_getv(view, MapBlocksPerTexel) - scale / 3;
        zox_tst_mp_stage = 3;
        zox_tst_mp_frames = 0;
        return 0;
    }
    if (zox_tst_mp_stage == 3) {
        if (zox_tst_mp_frames < 5) {
            return 0;
        }
        const int2 drawn = zox_getv(view, MapDrawnAt);
        zox_tst_mp_moved = drawn.x == zox_tst_mp_drawn.x + 1 && drawn.y == zox_tst_mp_drawn.y;
        // east is texel +1 (-1 flipped): now each texel shows what its neighbour did, where both were land
        const TextureData* data = zox_get(view, TextureData);
        const int step = zox_maps_flip_x ? -1 : 1, n = zox_tst_mp_texels;
        for (int ty = 0; ty < n; ty++) {
            for (int tx = 1; tx < n - 1; tx++) {
                const color now = data->value[tx + ty * n], before = zox_tst_mp_before[tx + step + ty * n];
                if ((!now.r && !now.g && !now.b) || (!before.r && !before.g && !before.b)) {
                    continue;
                }
                zox_tst_mp_compared++;
                zox_tst_mp_mismatched += now.r != before.r || now.g != before.g || now.b != before.b;
            }
        }
        // a whole redraw, timed
        TextureData scratch = { 0 };
        resize_TextureData(&scratch, n * n);
        const double start = current_time_in_seconds();
        draw_map_view(world, terrain, int2_multiply1(drawn, zox_getv(view, MapBlocksPerTexel)), n, zox_getv(view, MapBlocksPerTexel), &scratch);
        zox_tst_mp_draw_ms = (current_time_in_seconds() - start) * 1000;
        resize_TextureData(&scratch, 0);
        // a chunk under the player changes
        const int chunk_length = octree_size(zox_getv(terrain, NodeDepth));
        const int3 chunk_at = real_position_to_chunk_position(zox_tst_mp_at, chunk_length, scale);
        const entity tunk = int2_hashmap_get(zox_get(terrain, TunkLinks)->value, (int2) { chunk_at.x, chunk_at.z });
        const Chunk3Stack* stack = zox_valid(tunk) ? zox_get(tunk, Chunk3Stack) : NULL;
        const entity chunk = stack ? stack->value[0] : 0;
        zox_tst_mp_version = zox_tst_mp_maps_version(world, terrain);
        if (zox_valid(chunk)) {
            zox_add(chunk, VoxelNodeDirty);
        }
        zox_tst_mp_stage = 4;
        zox_tst_mp_frames = 0;
        return 0;
    }
    if (zox_tst_mp_frames == 1) {
        zox_tst_mp_cleared = zox_tst_mp_maps_version(world, terrain) > zox_tst_mp_version;
    }
    if (zox_tst_mp_frames < 5) {
        return 0;
    }
    zox_tst_mp_redrawn = zox_getv(view, MapDrawnVersion) == zox_tst_mp_maps_version(world, terrain);
    return 1;
}

void zox_tst_map_pixel_check(ecs* world) {
    (void) world;
    zox_log("[tst] [metric] map_pixel [%i] texels a side, a whole redraw [%.3f ms], [%i] of [%i] texels shifted wrong",
        zox_tst_mp_texels, zox_tst_mp_draw_ms, zox_tst_mp_mismatched, zox_tst_mp_compared);
    zox_tst_check(zox_tst_mp_still, "moving a third of a block redrew the minimap");
    zox_tst_check(zox_tst_mp_moved, "moving a block east didn't redraw the minimap a texel over");
    zox_tst_check(zox_tst_mp_compared > zox_tst_mp_texels * 4, "only [%i] texels of land to compare", zox_tst_mp_compared);
    zox_tst_check(!zox_tst_mp_mismatched, "[%i] of [%i] texels aren't their neighbour's from before", zox_tst_mp_mismatched,
        zox_tst_mp_compared);
    zox_tst_check(zox_tst_mp_cleared, "a chunk changing didn't clear its tunk's map colours (TunkMapsVersion)");
    zox_tst_check(zox_tst_mp_redrawn, "the minimap didn't redraw after a tunk's colours changed");
}
