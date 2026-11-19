// tst: coop_map_markers, split screen: the second player on the first one's minimap (map_marker_system)
//  - the second player's character held at an offset from the first's, its marker on the first's minimap checked:
//      4 blocks east, right of the middle; 4 blocks north (+z), up or down as the map's flipped (zox_maps_flip_z);
//      far east, on the map's right edge, level with the middle; then flying away (6000 blocks over 300 frames)
//      without a crash
//  - run: bash bsh/tst.sh coop_map_markers
int zox_tst_mm_stage = 0;
int zox_tst_mm_frames = 0;
int2 zox_tst_mm_east = { 999, 999 }, zox_tst_mm_north = { 999, 999 }, zox_tst_mm_far = { 999, 999 };
int2 zox_tst_mm_view_size;
float zox_tst_mm_ppb = 0;
entity zox_tst_mm_marker = 0;

// under e (a canvas), the first with a tag, a few deep
static inline entity zox_tst_mm_find(ecs* world, entity e, entity tag, byte depth) {
    if (zox_has_id(e, tag)) {
        return e;
    }
    entity found = 0;
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count && !found && depth < 4; i++) {
            found = zox_tst_mm_find(world, it.entities[i], tag, depth + 1);
        }
    }
    return found;
}

byte zox_tst_coop_map_markers_ready(ecs* world) {
    if (!zox_tst_coop_playing(world)) {
        return 0;
    }
    const entity a = zox_get_link(world, zox_players[0], CharacterLink);
    const entity b = zox_get_link(world, zox_players[1], CharacterLink);
    const entity canvas = zox_get_link(world, zox_players[0], CanvasLink);
    if (!zox_valid(zox_tst_mm_marker)) {
        zox_tst_mm_marker = zox_tst_mm_find(world, canvas, zox_id(MapMarker), 0);
        zox_tst_waiting = "a marker on the first player's minimap";
        return 0;
    }
    const entity view = zox_tst_mm_find(world, canvas, zox_id(MapView), 0);
    const entity terrain = zox_get_link(world, a, TerrainLink);
    if (!zox_valid(view) || !zox_getv(view, TextureSize).x) {
        zox_tst_waiting = "the minimap's view";
        return 0;
    }
    const float scale = zox_getv(terrain, BlockScale);
    zox_tst_mm_view_size = zox_getv(view, LayoutSize);
    zox_tst_mm_ppb = (float) zox_tst_mm_view_size.x / (zox_getv(view, TextureSize).x * zox_getv(view, MapBlocksPerTexel));
    // the second held at an offset (blocks) from the first, up high so it doesn't fall
    const float3 offsets[4] = { { 4, 0, 0 }, { 0, 0, 4 }, { 500, 0, 0 }, { 0, 0, 0 } };
    const int stage = zox_tst_mm_stage;
    const float3 p = zox_getv(a, Position3D);
    if (stage == 3) {
        // flying away: its own minimap and stream move off the first's land (a crash, play test 2026-11-19)
        const float away = 20.0f * zox_tst_mm_frames;
        zox_setv(b, Position3D, ((float3) { p.x + away * scale, p.y, p.z + away * 0.5f * scale }));
        zox_setv(b, Velocity3D, float3_zero);
        zox_tst_progress(stage * 100 + zox_tst_mm_frames);
        zox_tst_waiting = "the second player flying away";
        return ++zox_tst_mm_frames >= 300 ? ++zox_tst_mm_stage : 0;
    }
    zox_setv(b, Position3D, ((float3) { p.x + offsets[stage].x * scale, p.y, p.z + offsets[stage].z * scale }));
    zox_setv(b, Velocity3D, float3_zero);
    zox_tst_progress(stage * 100 + zox_tst_mm_frames);
    zox_tst_waiting = "the marker to move";
    if (++zox_tst_mm_frames < 20) {
        return 0;
    }
    const int2 at = zox_getv(zox_tst_mm_marker, LayoutPosition);
    if (stage == 0) {
        zox_tst_mm_east = at;
    } else if (stage == 1) {
        zox_tst_mm_north = at;
    } else {
        zox_tst_mm_far = at;
    }
    zox_tst_mm_frames = 0;
    zox_tst_mm_stage++;
    return 0;
}

void zox_tst_coop_map_markers_check(ecs* world) {
    const int step = (int) roundf(4 * zox_tst_mm_ppb);
    const int north = zox_maps_flip_z ? -step : step;
    const int edge = zox_tst_mm_view_size.x / 2 - zox_getv(zox_tst_mm_marker, LayoutSize).x / 2;
    zox_log("[tst] map [%i] pixels across, [%.2f] a block: east [%i %i], north [%i %i], far east [%i %i] (edge [%i])",
        zox_tst_mm_view_size.x, zox_tst_mm_ppb, zox_tst_mm_east.x, zox_tst_mm_east.y, zox_tst_mm_north.x, zox_tst_mm_north.y,
        zox_tst_mm_far.x, zox_tst_mm_far.y, edge);
    zox_tst_check(step > 0, "a map block is [%.2f] pixels", zox_tst_mm_ppb);
    zox_tst_check(abs(zox_tst_mm_east.x - step) <= 1 && abs(zox_tst_mm_east.y) <= 1,
        "4 blocks east the marker's at [%i %i], not [%i 0]", zox_tst_mm_east.x, zox_tst_mm_east.y, step);
    zox_tst_check(abs(zox_tst_mm_north.x) <= 1 && abs(zox_tst_mm_north.y - north) <= 1,
        "4 blocks north the marker's at [%i %i], not [0 %i]", zox_tst_mm_north.x, zox_tst_mm_north.y, north);
    zox_tst_check(abs(zox_tst_mm_far.x - edge) <= 1 && abs(zox_tst_mm_far.y) <= 1,
        "far east the marker's at [%i %i], not on the edge [%i 0]", zox_tst_mm_far.x, zox_tst_mm_far.y, edge);
}
