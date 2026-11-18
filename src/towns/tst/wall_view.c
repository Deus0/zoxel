// tst: wall_view, the readme's screenshot (tst/wall_view.ppm, --nohud from tst.sh): the camera freed onto a town's
//  wall top, looking in over the player standing in the town and on to the land past it
//  - run: zox_tst_seed=<seed> bash bsh/tst.sh wall_view --1080p --far 16 --nouis (seed 2 made it), then: magick tst/wall_view.ppm -resize 50% -quality 88 doc/screenshot.jpg
byte zox_tst_wall_view_stage = 0;
double zox_tst_wall_view_time = 0;
entity zox_tst_wall_view_town = 0;

// the highest solid voxel in a column (unloaded chunks skipped), -1 if none
static inline int zox_tst_wall_view_top(ecs* world, entity terrain, int2 column) {
    for (int y = 255; y >= 0; y--) {
        const int voxel = terrain_voxel_at(world, terrain, (int3) { column.x, y, column.y });
        if (voxel > 0) {
            return y;
        }
    }
    return -1;
}

byte zox_tst_wall_view_ready(ecs* world) {
    if (!zox_tst_wall_view_stage) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        const entity region = zox_tst_player_region(world);
        entity towns[zox_children_capacity];
        const uint count = zox_valid(region) ? zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town)) : 0;
        if (!count) {
            zox_tst_check(0, "no town in the player's region");
            return 1;
        }
        zox_tst_wall_view_town = towns[0];
        zox_tst_wall_view_stage = 1;
        zox_tst_wall_view_time = zox_current_time;
    }
    zox_tst_waiting = "the wall view";
    zox_tst_progress(zox_tst_wall_view_stage * 100 + (int) (zox_current_time - zox_tst_wall_view_time));
    const entity character = zox_tst_player_character(world);
    const float scale = zox_getv(zox_get_link(world, character, TerrainLink), BlockScale);
    const entity town = zox_tst_wall_view_town;
    const int2 center = zox_getv(town, BlockPosition2);
    const byte2 size = zox_getv(town, TownSize);
    // the player a little in from the back wall, the camera on that wall behind and above them, both on the top
    //  voxels there (the land under a town is flattened, the heights function doesn't know it)
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const int back = center.y - size.y / 2;
    const int thick = zox_getv(town, WallThickness);
    const int2 stand = { center.x, back + thick + 12 }, perch = { center.x + 2, back + thick - 1 };
    int ground = zox_tst_wall_view_top(world, terrain, stand), wall_top = zox_tst_wall_view_top(world, terrain, perch);
    if (ground < 0 || wall_top < 0) {
        // up high until they load
        ground = wall_top = 100;
        zox_tst_wall_view_time = zox_current_time;
    }
    const float3 player = { (stand.x + 0.5f) * scale, (ground + 1) * scale, (stand.y + 0.5f) * scale };
    const float3 eye = { (perch.x + 0.5f) * scale, (wall_top + 6) * scale, (perch.y + 0.5f) * scale };
    const float3 target = { player.x, player.y + 2 * scale, player.z + 40 * scale };
    zox_setv(character, Position3D, player);
    zox_setv(character, Velocity3D, float3_zero);
    zox_setv(character, RenderDisabled, 0);
    zox_setv(character, Rotation3D, quaternion_from_euler((float3) { 0, M_PI, 0 }));
    const entity camera = zox_get_link(world, dbg_player, CameraLink);
    if (zox_valid(camera)) {
        // freed as free roam does (controllers3/sys/freeroam.c), its rotation set here
        if (zox_get_parent(world, camera)) {
            zox_set_parent(world, camera, 0);
        }
        zox_setv(camera, Position3D, eye);
        zox_setv(camera, Rotation3D, camera_look_at(eye, target));
    }
    fog_density = 0.004f;
    if (zox_tst_wall_view_stage == 1) {
        if (zox_current_time - zox_tst_wall_view_time < 25) {
            return 0;
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("wall_view.ppm"));
        zox_tst_wall_view_stage = 2;
        return 0;
    }
    return !zox_screenshot_request[0];
}

void zox_tst_wall_view_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_clouds_share(zox_tst_path("wall_view.ppm")) >= 0, "no screenshot saved");
}
