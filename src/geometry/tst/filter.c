// tst: camera_filter, which camera draws what: CameraFilter cameras only draw what's linked to them
//  (CameraRenderer), other cameras draw everything not linked to another camera
//  - 3 viewport cameras looking at 2 cubes: the left linked to camera A, the right linked to none
//      A (filtered): left, not right; B (filtered, nothing linked): neither; C (unfiltered): right, not left
//  - checked on screenshots, before and after the cubes, at where each cube lands in each view
//  - what model_views leans on (a copy of the model per view); also mirrors, minimaps, portraits
//  - saves tst/camera_filter.ppm; run: bash bsh/tst.sh camera_filter
extern char zox_screenshot_request[256];
extern byte nohud;
#define zox_tst_filter_cameras 3
const char* zox_tst_filter_names[zox_tst_filter_cameras] = { "A filtered", "B filtered, nothing linked", "C unfiltered" };
// [camera][cube]: left, right
const byte zox_tst_filter_expected[zox_tst_filter_cameras][2] = { { 1, 0 }, { 0, 0 }, { 0, 1 } };
const float3 zox_tst_filter_target = { 0, 1000, 0 };   // away from the menu's scene
const float zox_tst_filter_offset = 0.8f;              // each cube off the middle
byte zox_tst_filter_stage = 0;
int zox_tst_filter_frames = 0;
entity zox_tst_filter_camera_list[zox_tst_filter_cameras];
entity zox_tst_filter_cubes[2];

static inline float3 zox_tst_filter_cube_position(int cube) {
    return float3_add(zox_tst_filter_target, (float3) { cube ? zox_tst_filter_offset : -zox_tst_filter_offset, 0, 0 });
}

byte zox_tst_camera_filter_ready(ecs* world) {
    zox_tst_filter_frames++;
    zox_tst_progress(zox_tst_filter_stage * 1000 + zox_tst_filter_frames);
    if (zox_tst_filter_stage == 0) {
        zox_tst_waiting = "the app";
        if (zox_tst_filter_frames < 10 || !zox_valid(main_app)) {
            return 0;
        }
        const float3 position = float3_add(zox_tst_filter_target, (float3) { 0, 0, 4 });
        for (int i = 0; i < zox_tst_filter_cameras; i++) {
            const entity camera = spawn_viewport_camera(
                world,
                main_app,
                position,
                camera_look_at(position, zox_tst_filter_target),
                50,
                viewport_grid_cell(i, 2, 2));
            if (i < 2) {
                zox_add(camera, CameraFilter);
            }
            zox_setv(camera, Color, ((color) { 96, 96, 96, 255 }));
            zox_tst_filter_camera_list[i] = camera;
        }
        // only the views on the screen
        nohud = 1;
        const entity game_view = zox_valid(main_cameras[0]) ? zox_get_link(world, main_cameras[0], RenderTextureLink) : 0;
        if (zox_valid(game_view)) {
            zox_setv(game_view, RenderDisabled, 1);
        }
        zox_tst_filter_stage = 1;
        zox_tst_filter_frames = 0;
        return 0;
    }
    if (zox_tst_filter_stage == 1) {
        zox_tst_waiting = "the empty views' screenshot";
        if (zox_tst_filter_frames == 10) {
            snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("camera_filter_empty.ppm"));
        }
        if (zox_tst_filter_frames <= 10 || zox_screenshot_request[0]) {
            return 0;
        }
        for (int c = 0; c < 2; c++) {
            const entity cube = spawn_cube(world, prefab_cube, zox_tst_filter_cube_position(c), 0.5f);
            if (!cube) {
                zox_tst_check(0, "couldn't spawn cube [%i]", c);
                return 1;
            }
            zox_setv(cube, Color, ((color) { 220, 40, 40, 255 }));
            zox_tst_filter_cubes[c] = cube;
        }
        zox_link(world, zox_tst_filter_cubes[0], CameraRenderer, zox_tst_filter_camera_list[0]);
        zox_tst_filter_stage = 2;
        zox_tst_filter_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the views' screenshot";
    if (zox_tst_filter_stage == 2) {
        if (zox_tst_filter_frames < 10) {
            return 0;
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("camera_filter.ppm"));
        zox_tst_filter_stage = 3;
        return 0;
    }
    return !zox_screenshot_request[0];
}

void zox_tst_camera_filter_check(ecs* world) {
    int2 size;
    int2 size_empty;
    byte* empty = zox_tst_load_ppm(zox_tst_path("camera_filter_empty.ppm"), &size_empty);
    byte* pixels = zox_tst_load_ppm(zox_tst_path("camera_filter.ppm"), &size);
    zox_tst_check(empty && pixels && int2_equals(size, size_empty), "no screenshots, or of different sizes");
    if (empty && pixels && int2_equals(size, size_empty)) {
        for (int i = 0; i < zox_tst_filter_cameras; i++) {
            const entity camera = zox_tst_filter_camera_list[i];
            const float4 cell = viewport_grid_cell(i, 2, 2);
            const int2 cell_position = screen_to_canvas_position(size, cell);
            const int2 cell_size = screen_to_canvas_size(size, cell);
            const float4x4 view_projection = zox_getv(camera, ViewProjectionMatrix);
            for (int c = 0; c < 2; c++) {
                // where the cube's middle lands in this view, in the screenshot (rows from the top)
                const float4 seen = camera_project_point(view_projection, zox_tst_filter_cube_position(c));
                const int x = cell_position.x + (int) ((seen.x + 1) * 0.5f * cell_size.x);
                const int y = size.y - 1 - (cell_position.y + (int) ((seen.y + 1) * 0.5f * cell_size.y));
                int changed = 0;
                int total = 0;
                for (int dy = -2; dy <= 2; dy++) {
                    for (int dx = -2; dx <= 2; dx++) {
                        if (x + dx < 0 || y + dy < 0 || x + dx >= size.x || y + dy >= size.y) {
                            continue;
                        }
                        changed += zox_tst_pixel_difference(
                            zox_tst_ppm_pixel(pixels, size, x + dx, y + dy),
                            zox_tst_ppm_pixel(empty, size, x + dx, y + dy)) > 24;
                        total++;
                    }
                }
                const byte drawn = total && changed * 2 > total;
                const byte expected = zox_tst_filter_expected[i][c];
                zox_tst_check(drawn == expected, "camera [%s] %s the %s cube (%s), [%i/%i] pixels changed at [%ix%i]",
                    zox_tst_filter_names[i], drawn ? "drew" : "didn't draw", c ? "right" : "left",
                    c ? "linked to none" : "linked to A", changed, total, x, y);
            }
        }
    }
    free(empty);
    free(pixels);
}

void zox_tsts_geometry() {
    zox_tst_add("camera_filter", zox_tst_camera_filter_ready, zox_tst_camera_filter_check, 30);
    zox_tst_set_setup("camera_filter", NULL, 1);
}
