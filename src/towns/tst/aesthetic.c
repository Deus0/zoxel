// tst: aesthetic_town, the readme's screenshots from a town's wall, to refresh them as the game grows
//  - the town nearest the player, the wall across from its gate, the camera freed (as free roam does) onto it:
//      wall: along the wall top, the player standing on it, the town to one side, the wild to the other
//      inside: from the wall's inner edge, over the town and on to the far wall
//      outside: from its outer edge, out over the land
//  - each to tst/aesthetic/<shot>.ppm, bsh/tst.sh converts them to doc/screenshot/<shot>.jpg
//  - run: bash bsh/tst.sh aesthetic_town (tst.sh adds a far, tall world, 1080p, no hud or labels, seed 2)
#define zox_tst_aesthetic_shots 3
const char* zox_tst_aesthetic_names[zox_tst_aesthetic_shots] = { "wall", "inside", "outside" };
byte zox_tst_aesthetic_stage = 0;
double zox_tst_aesthetic_time = 0;
entity zox_tst_aesthetic_town = 0;

// the highest solid voxel in a column (unloaded chunks skipped), -1 if none
static inline int zox_tst_aesthetic_top(ecs* world, entity terrain, int2 column) {
    for (int y = 255; y >= 0; y--) {
        if (terrain_voxel_at(world, terrain, (int3) { column.x, y, column.y }) > 0) {
            return y;
        }
    }
    return -1;
}

// a column's top as a position, a block over it (its middle), -1 tops held high until they load
static inline float3 zox_tst_aesthetic_at(ecs* world, entity terrain, int2 column, float above, float scale) {
    int top = zox_tst_aesthetic_top(world, terrain, column);
    if (top < 0) {
        top = 120;
        zox_tst_aesthetic_time = zox_current_time;
    }
    return (float3) { (column.x + 0.5f) * scale, (top + 1 + above) * scale, (column.y + 0.5f) * scale };
}

static inline int2 zox_tst_aesthetic_step(int2 from, int2 direction, int steps) {
    return (int2) { from.x + direction.x * steps, from.y + direction.y * steps };
}

// the town nearest the player
static inline entity zox_tst_aesthetic_find_town(ecs* world, entity character, float scale) {
    const entity region = zox_tst_player_region(world);
    entity towns[zox_children_capacity];
    const uint count = zox_valid(region) ? zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town)) : 0;
    const float3 p = zox_getv(character, Position3D);
    const int2 player = { p.x / scale, p.z / scale };
    entity nearest = 0;
    for (uint i = 0; i < count; i++) {
        if (!nearest || int2_distance(zox_getv(towns[i], BlockPosition2), player) < int2_distance(zox_getv(nearest, BlockPosition2), player)) {
            nearest = towns[i];
        }
    }
    return nearest;
}

byte zox_tst_aesthetic_town_ready(ecs* world) {
    const entity character = zox_tst_player_character(world);
    if (!zox_tst_aesthetic_stage) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_aesthetic_town = zox_tst_aesthetic_find_town(world, character, zox_getv(zox_get_link(world, character, TerrainLink), BlockScale));
        if (!zox_valid(zox_tst_aesthetic_town)) {
            zox_tst_check(0, "no town in the player's region");
            return 1;
        }
        zox_tst_aesthetic_stage = 1;
        zox_tst_aesthetic_time = zox_current_time;
    }
    zox_tst_waiting = "the aesthetic shots";
    zox_tst_progress(zox_tst_aesthetic_stage * 100 + (int) (zox_current_time - zox_tst_aesthetic_time));
    if (zox_screenshot_request[0]) {
        return 0;
    }
    const byte shot = zox_tst_aesthetic_stage - 1;
    if (shot >= zox_tst_aesthetic_shots) {
        return 1;
    }
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const float scale = zox_getv(terrain, BlockScale);
    const entity town = zox_tst_aesthetic_town;
    const int2 center = zox_getv(town, BlockPosition2);
    const byte2 size = zox_getv(town, TownSize);
    const int thick = zox_getv(town, WallThickness);
    // the wall across from the gate (sides: 0 +z, 1 +x, 2 -z, 3 -x), out its normal, along it its tangent
    const byte side = (zox_getv(town, TownGateSide) + 2) % 4;
    const int2 normals[4] = { { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 } };
    const int2 out = normals[side], along = { -out.y, out.x };
    const int half = out.x ? size.x / 2 : size.y / 2;
    const int2 middle = zox_tst_aesthetic_step(center, out, half - thick / 2);
    const int2 inner = zox_tst_aesthetic_step(center, out, half - thick + 1), outer = zox_tst_aesthetic_step(center, out, half);
    // the player on the wall top, back to the camera, looking along it
    const float3 player = zox_tst_aesthetic_at(world, terrain, zox_tst_aesthetic_step(middle, along, -15), 0, scale);
    zox_setv(character, Position3D, player);
    zox_setv(character, Velocity3D, float3_zero);
    zox_setv(character, RenderDisabled, 0);
    zox_setv(character, Rotation3D, camera_look_at(float3_zero, (float3) { -along.x, 0, -along.y }));
    float3 eye, target;
    if (shot == 0) {
        eye = zox_tst_aesthetic_at(world, terrain, zox_tst_aesthetic_step(middle, along, -20), 5, scale);
        target = zox_tst_aesthetic_at(world, terrain, zox_tst_aesthetic_step(middle, along, 24), -4, scale);
    } else {
        const int2 from = shot == 1 ? inner : outer, facing = shot == 1 ? (int2) { -out.x, -out.y } : out;
        eye = zox_tst_aesthetic_at(world, terrain, zox_tst_aesthetic_step(from, along, 6), 6, scale);
        target = float3_add(eye, (float3) { facing.x * 40 * scale, -6 * scale, facing.y * 40 * scale });
    }
    const entity camera = zox_get_link(world, dbg_player, CameraLink);
    if (zox_valid(camera)) {
        if (zox_get_parent(world, camera)) {
            zox_set_parent(world, camera, 0);
        }
        zox_setv(camera, Position3D, eye);
        zox_setv(camera, Rotation3D, camera_look_at(eye, target));
    }
    fog_density = 0.003f;
    // the first waits for the far land, the others for the camera to settle
    if (zox_current_time - zox_tst_aesthetic_time < (shot ? 3 : 30)) {
        return 0;
    }
    char name[64];
    snprintf(name, sizeof(name), "aesthetic/%s.ppm", zox_tst_aesthetic_names[shot]);
    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(name));
    zox_tst_aesthetic_stage++;
    zox_tst_aesthetic_time = zox_current_time;
    return 0;
}

void zox_tst_aesthetic_town_check(ecs* world) {
    (void) world;
    for (int i = 0; i < zox_tst_aesthetic_shots; i++) {
        char name[64];
        snprintf(name, sizeof(name), "aesthetic/%s.ppm", zox_tst_aesthetic_names[i]);
        zox_tst_check(zox_tst_clouds_share(zox_tst_path(name)) >= 0, "no [%s] screenshot saved", zox_tst_aesthetic_names[i]);
    }
}
