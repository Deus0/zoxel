// tst: glow_view, a glowing block (BlockGlow: glow stones) draws brighter than white (doc/plans/hdr.md): its faces'
//  colour alpha on the cpu (lights3 build_smooth), the draw's glows by voxel on the gpu (lights3 gpu_mesh)
//  - a chunk3 parent's top middle chunk (chunk3_parent.c): a glow stone alone in the sky's air
//  - a camera of its own straight to the screen, a voxel and a half in front of it, the game's view hidden
//  - its warm colour (255 226 150): unlit by glow its blue is at most its texture's (~150); glowing it's past
//    white, clipped: its middle's blue near 255
//  - run: bash bsh/tst.sh glow_view
extern char zox_screenshot_request[256];
byte zox_tst_gview_stage = 0;
int zox_tst_gview_frames = 0;

byte zox_tst_glow_view_ready(ecs* world) {
    if (!zox_tst_chunk3_parent_ready(world)) {
        return 0;
    }
    const int size = octree_size(terrain_depth);
    const byte3 local = { 5, 7, 5 };
    if (zox_tst_gview_stage == 0) {
        const byte glow = zox_tst_chunks_block(world, zox_block_material_solid, 230);
        zox_tst_check(glow, "the realm has no glow stone (solid, light 230)");
        VoxelNodeQueue* queue = zox_getm(zox_tst_chunk_at(world, (int3) { 0, 1, 0 }), VoxelNodeQueue);
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = glow, .position = local });
        zox_tst_gview_stage = 1;
        return 0;
    }
    if (zox_tst_gview_stage == 1) {
        // its middle, the chunk (0, 1, 0)'s voxel at local, half a unit a voxel (the parent's 0.5)
        const float3 target = voxel_to_real_position((int3) { local.x, size + local.y, local.z }, 0.5f, 0.5f);
        const float3 eye = float3_add(target, (float3) { 0, 0.2f, 0.75f });
        const entity camera = spawn_viewport_camera(world, main_app, eye, camera_look_at(eye, target), 60,
            viewport_grid_cell(0, 1, 1));
        zox_setv(camera, Color, ((color) { 20, 20, 30, 255 }));
        nohud = 1;
        const entity game_view = zox_get_link(world, main_cameras[0], RenderTextureLink);
        if (zox_valid(game_view)) {
            zox_setv(game_view, RenderDisabled, 1);
        }
        zox_tst_gview_stage = 2;
        return 0;
    }
    if (zox_tst_gview_stage == 2) {
        zox_tst_waiting = "the view drawn";
        if (++zox_tst_gview_frames < 10) {
            return 0;
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("glow_view.ppm"));
        zox_tst_gview_stage = 3;
        return 0;
    }
    zox_tst_waiting = "the screenshot";
    return !zox_screenshot_request[0];
}

void zox_tst_glow_view_check(ecs* world) {
    (void) world;
    int2 size;
    byte* image = zox_tst_load_ppm(zox_tst_path("glow_view.ppm"), &size);
    zox_tst_check(image, "glow_view.ppm wasn't saved");
    if (!image) {
        return;
    }
    int r = 0, g = 0, b = 0, count = 0;
    for (int i = -4; i <= 4; i++) for (int j = -4; j <= 4; j++) {
        const byte* p = zox_tst_ppm_pixel(image, size, size.x / 2 + i, size.y / 2 + j);
        r += p[0];
        g += p[1];
        b += p[2];
        count++;
    }
    free(image);
    r /= count;
    g /= count;
    b /= count;
    zox_log("[tst] glow_view: the glow stone's middle [%i %i %i]", r, g, b);
    zox_tst_check(r > 200 && g > 180, "the glow stone isn't in view, or dark [%i %i %i]", r, g, b);
    zox_tst_check(b > 220, "the glow stone's blue [%i]: not past white (its texture's is ~150 unglowing)", b);
}

// tst: glow_views, a glow stone in a dark brick room through the game's camera and its post pass, four views
//  (bsh/tools/view_glow.sh saves them as pngs to look at): clamp, bloom, soft tonemap with bloom, filmic with bloom
//  - a chunk3 parent's top middle chunk: a closed brick room (inside 4 to 10, floor y 3, roof y 10), no sky in, a glow
//    stone on its floor in the middle: lit by it alone
//  - the menu's camera (a render camera: tonemap, bloom) inside by a wall, looking down at it, ui hidden
//  - checks: each view saved; the glow stone brighter than the room's far corner; bloom brightens the area round it
//  - run: bash bsh/tst.sh glow_views, or bash bsh/tools/view_glow.sh
typedef struct {
    byte tonemap;
    byte bloom;
    const char* file;
} zox_tst_gviews_view;

const zox_tst_gviews_view zox_tst_gviews_views[4] = {
    { 0, 0, "glow_clamp.ppm" },
    { 0, 1, "glow_bloom.ppm" },
    { 1, 1, "glow_soft_bloom.ppm" },
    { 2, 1, "glow_filmic_bloom.ppm" } };
byte zox_tst_gviews_stage = 0;
byte zox_tst_gviews_view_index = 0;
int zox_tst_gviews_frames = 0;

byte zox_tst_glow_views_ready(ecs* world) {
    if (!zox_tst_chunk3_parent_ready(world)) {
        return 0;
    }
    const int size = octree_size(terrain_depth);
    if (zox_tst_gviews_stage == 0) {
        const byte bricks = zox_tst_chunks_block(world, zox_block_material_solid, 0);
        const byte glow = zox_tst_chunks_block(world, zox_block_material_solid, 230);
        zox_tst_check(glow, "the realm has no glow stone (solid, light 230)");
        VoxelNodeQueue* queue = zox_getm(zox_tst_chunk_at(world, (int3) { 0, 1, 0 }), VoxelNodeQueue);
        for (byte x = 3; x <= 11; x++) for (byte y = 3; y <= 10; y++) for (byte z = 3; z <= 11; z++) {
            const byte inside = x >= 4 && x <= 10 && y >= 4 && y <= 9 && z >= 4 && z <= 10;
            if (!inside) {
                a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = bricks, .position = (byte3) { x, y, z } });
            }
        }
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = glow, .position = (byte3) { 7, 4, 7 } });
        zox_tst_gviews_stage = 1;
        return 0;
    }
    const entity camera = main_cameras[0];
    if (zox_tst_gviews_stage == 1) {
        const float3 target = voxel_to_real_position((int3) { 7, size + 4, 7 }, 0.5f, 0.5f);
        const float3 eye = voxel_to_real_position((int3) { 7, size + 8, 10 }, 0.5f, 0.5f);
        zox_tst_lids_make_void(world, camera);
        zox_setv(camera, Color, ((color) { 0, 0, 0, 255 }));
        zox_setv(camera, Position3D, eye);
        zox_setv(camera, Rotation3D, camera_look_at(eye, target));
        nohud = 1;
        zox_tst_gviews_stage = 2;
        return 0;
    }
    zox_tst_waiting = "the glow's views";
    zox_tst_progress(zox_tst_gviews_view_index * 1000 + zox_tst_gviews_frames);
    if (zox_screenshot_request[0]) {
        return 0;
    }
    if (zox_tst_gviews_view_index >= 4) {
        return 1;
    }
    const zox_tst_gviews_view* view = &zox_tst_gviews_views[zox_tst_gviews_view_index];
    if (++zox_tst_gviews_frames == 1) {
        zox_set_tonemap(world, view->tonemap);
        zox_set_bloom(world, view->bloom);
        return 0;
    }
    if (zox_tst_gviews_frames < 15) {
        return 0;
    }
    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(view->file));
    zox_tst_gviews_view_index++;
    zox_tst_gviews_frames = 0;
    return 0;
}

// the mean brightness (0 to 765) of the image's middle, from 0.5 - half to 0.5 + half
static inline int zox_tst_gviews_area(const byte* pixels, int2 size, float half) {
    long sum = 0;
    int count = 0;
    for (int x = (int) ((0.5f - half) * size.x); x < (int) ((0.5f + half) * size.x); x++) {
        for (int y = (int) ((0.5f - half) * size.y); y < (int) ((0.5f + half) * size.y); y++) {
            const byte* p = zox_tst_ppm_pixel(pixels, size, x, y);
            sum += p[0] + p[1] + p[2];
            count++;
        }
    }
    return count ? (int) (sum / count) : 0;
}

void zox_tst_glow_views_check(ecs* world) {
    (void) world;
    int middle[4] = { 0 }, beside[4] = { 0 }, corner[4] = { 0 };
    for (byte i = 0; i < 4; i++) {
        int2 size;
        byte* image = zox_tst_load_ppm(zox_tst_path(zox_tst_gviews_views[i].file), &size);
        zox_tst_check(image, "[%s] wasn't saved", zox_tst_gviews_views[i].file);
        if (!image) {
            return;
        }
        middle[i] = zox_tst_lids_at(image, size, 0.5f, 0.5f);
        beside[i] = zox_tst_gviews_area(image, size, 0.15f);    // round it, its glow
        corner[i] = zox_tst_lids_at(image, size, 0.03f, 0.03f);
        free(image);
    }
    zox_log("[tst] glow_views: the glow stone [%i %i %i %i], round it [%i %i %i %i], a corner [%i %i %i %i]",
        middle[0], middle[1], middle[2], middle[3], beside[0], beside[1], beside[2], beside[3],
        corner[0], corner[1], corner[2], corner[3]);
    zox_tst_check(middle[0] > corner[0] + 100, "the glow stone [%i] isn't brighter than the room's corner [%i]",
        middle[0], corner[0]);
    zox_tst_check(beside[1] >= beside[0] + 3, "bloom didn't brighten round the glow stone [%i to %i]", beside[0], beside[1]);
}
