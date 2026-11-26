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
