// Camera and viewport tests
//  - camera_viewports: viewport rects from screen_to_canvas, grids of them tile the window exactly
//  - camera_projection: the perspective matrix puts the fov's edges on the viewport's edges
//  - camera_look_at: camera_look_at turns a camera so its target lands in the middle of its view, y up
//  - camera_viewport_spawn: spawn_viewport_camera in a 2x2 grid, sized, projected, resized with
//      the window, and forgotten by the app once deleted
//  - camera_orthographic: the orthographic projection, a unit high view, no perspective
//  - camera_split_screen: --splitscreen's two players, cameras and canvases in their halves, resized
//  - camera_downscale: viewport_downscale 4 (web), render cameras at a quarter, screen cameras full size
//  - camera_frustum (src/streaming/tst/frustum.c): which boxes a camera sees, the chunk culling check
//  - camera_filter (src/geometry/tst/filter.c): CameraFilter / CameraRenderer, who draws what
//  - run: bash bsh/tst.sh camera_viewports (pure logic ones are quick, on the main menu)
//  - the visual one: tst model_views (src/controllers/tst/model_views.c)

#define zox_tst_camera_epsilon 0.001f

byte zox_tst_camera_ready_now(ecs* world) {
    (void) world;
    return 1;
}

// the view projection of a camera at position with rotation, the same maths as the camera systems
//  - far: its CameraFarDistance
float4x4 zox_tst_camera_view_projection(
    float3 position,
    float4 rotation,
    float aspect,
    float fov,
    float far)
{
    float4x4 projection;
    calculate_perspective_projection_matrix(&projection, aspect, camera_near_distance, far, fov);
    const float4x4 view = float4x4_inverse(float4x4_transform(position, rotation));
    return float4x4_multiply(view, projection);
}

// tst: camera_viewports
void zox_tst_camera_viewports_check(ecs* world) {
    (void) world;
    const int2 full_hd = { 1920, 1080 };
    // the whole screen
    int2 size = screen_to_canvas_size(full_hd, single_screen_to_canvas);
    int2 position = screen_to_canvas_position(full_hd, single_screen_to_canvas);
    zox_tst_check(int2_equals(size, full_hd), "single viewport size [%ix%i]", size.x, size.y);
    zox_tst_check(int2_equals(position, int2_zero), "single viewport position [%ix%i]", position.x, position.y);
    // a 2x2 grid: 0 top left, 1 top right, 2 bottom left, 3 bottom right (gl's y goes up)
    const int2 expected_positions[4] = { { 0, 540 }, { 960, 540 }, { 0, 0 }, { 960, 0 } };
    for (int i = 0; i < 4; i++) {
        const float4 cell = viewport_grid_cell(i, 2, 2);
        size = screen_to_canvas_size(full_hd, cell);
        position = screen_to_canvas_position(full_hd, cell);
        zox_tst_check(int2_equals(size, (int2) { 960, 540 }), "grid cell [%i] size [%ix%i]", i, size.x, size.y);
        zox_tst_check(int2_equals(position, expected_positions[i]), "grid cell [%i] at [%ix%i], expected [%ix%i]",
            i, position.x, position.y, expected_positions[i].x, expected_positions[i].y);
    }
    // grids tile odd sizes exactly: every pixel covered once
    const int2 odd_sizes[3] = { { 1001, 777 }, { 33, 17 }, { 1366, 768 } };
    const int2 grids[4] = { { 2, 2 }, { 3, 1 }, { 1, 3 }, { 3, 3 } };
    for (int s = 0; s < 3; s++) {
        const int2 screen = odd_sizes[s];
        for (int g = 0; g < 4; g++) {
            const int columns = grids[g].x;
            const int rows = grids[g].y;
            byte* covered = calloc((size_t) screen.x * screen.y, 1);
            int overlaps = 0;
            int outside = 0;
            for (int i = 0; i < columns * rows; i++) {
                const float4 cell = viewport_grid_cell(i, columns, rows);
                const int2 cell_size = screen_to_canvas_size(screen, cell);
                const int2 cell_position = screen_to_canvas_position(screen, cell);
                for (int x = cell_position.x; x < cell_position.x + cell_size.x; x++) {
                    for (int y = cell_position.y; y < cell_position.y + cell_size.y; y++) {
                        if (x < 0 || y < 0 || x >= screen.x || y >= screen.y) {
                            outside++;
                            continue;
                        }
                        if (covered[x + y * screen.x]++) {
                            overlaps++;
                        }
                    }
                }
            }
            int missed = 0;
            for (int p = 0; p < screen.x * screen.y; p++) {
                missed += !covered[p];
            }
            free(covered);
            zox_tst_check(!missed && !overlaps && !outside,
                "[%ix%i] grid on [%ix%i]: [%i] pixels missed, [%i] overlapped, [%i] outside",
                columns, rows, screen.x, screen.y, missed, overlaps, outside);
        }
    }
    // a bad grid falls back to the whole screen
    const float4 bad = viewport_grid_cell(0, 0, 2);
    zox_tst_check(float4_equals(bad, single_screen_to_canvas), "a 0 column grid isn't the whole screen");
}

// tst: camera_projection
void zox_tst_camera_projection_check(ecs* world) {
    (void) world;
    const float fovs[3] = { 45, 60, 90 };
    const float aspects[3] = { 1, 16 / 9.0f, 0.5f };
    float worst = 0;
    for (int f = 0; f < 3; f++) {
        for (int a = 0; a < 3; a++) {
            const float fov = fovs[f];
            const float aspect = aspects[a];
            float4x4 projection;
            calculate_perspective_projection_matrix(&projection, aspect, camera_near_distance, camera_far_distance, fov);
            // fov is vertical: a point on the fov's top edge, any distance, is at the viewport's top
            const float distance = 10;
            const float half_height = distance * tanf(fov * 0.5f * degreesToRadians);
            const float half_width = half_height * aspect;
            const float4 top = camera_project_point(projection, (float3) { 0, half_height, -distance });
            const float4 corner = camera_project_point(projection, (float3) { half_width, -half_height, -distance });
            const float4 centre = camera_project_point(projection, (float3) { 0, 0, -distance });
            const float4 behind = camera_project_point(projection, (float3) { 0, 0, distance });
            const float error = fabsf(top.y - 1) + fabsf(top.x) +
                fabsf(corner.x - 1) + fabsf(corner.y + 1) +
                fabsf(centre.x) + fabsf(centre.y);
            if (error > worst) {
                worst = error;
            }
            zox_tst_check(top.w > 0 && centre.z > -1 && centre.z < 1, "fov [%.0f] aspect [%.2f]: a point in front isn't in the depth range (w [%f] z [%f])",
                fov, aspect, top.w, centre.z);
            zox_tst_check(behind.w < 0, "fov [%.0f] aspect [%.2f]: a point behind the camera has w [%f]", fov, aspect, behind.w);
        }
    }
    zox_tst_check(worst < zox_tst_camera_epsilon, "fov edges off the viewport edges by [%f]", worst);
}

// tst: camera_look_at
void zox_tst_camera_look_at_check(ecs* world) {
    (void) world;
    // the identity looks down -z
    const float4 forward = camera_look_at(float3_zero, (float3) { 0, 0, -5 });
    zox_tst_check(float4_length(float4_subtract(forward, quaternion_identity)) < zox_tst_camera_epsilon,
        "looking down -z isn't the identity [%f %f %f %f]", forward.x, forward.y, forward.z, forward.w);
    // a camera looking at a target sees it in the middle, and what's above it higher up
    uint seed = 4321;
    float worst_centre = 0;
    int below = 0;
    int behind = 0;
    int not_unit = 0;
    const int samples = 500;
    for (int i = 0; i < samples; i++) {
        float values[6];
        for (int v = 0; v < 6; v++) {
            seed ^= seed << 13;
            seed ^= seed >> 17;
            seed ^= seed << 5;
            values[v] = (float) (seed % 20001) / 1000.0f - 10.0f;  // -10 to 10
        }
        float3 position = { values[0], values[1], values[2] };
        float3 target = { values[3], values[4], values[5] };
        // the four model_views angles, and straight down (yaw has no meaning there)
        if (i < 5) {
            const float3 views[5] = { { 0, 1, 6 }, { 6, 1, 0 }, { 0, 1, -6 }, { 4, 4, 4 }, { 0, 8, 0 } };
            target = float3_zero;
            position = views[i];
        }
        if (float3_length(float3_subtract(target, position)) < 0.5f) {
            continue;
        }
        const float4 rotation = camera_look_at(position, target);
        if (fabsf(float4_length(rotation) - 1) > zox_tst_camera_epsilon) {
            not_unit++;
        }
        const float4x4 view_projection = zox_tst_camera_view_projection(position, rotation, 16 / 9.0f, 60, camera_far_distance);
        const float4 seen = camera_project_point(view_projection, target);
        if (seen.w <= 0) {
            behind++;
            continue;
        }
        const float error = fabsf(seen.x) + fabsf(seen.y);
        if (error > worst_centre) {
            worst_centre = error;
        }
        // up stays up (when not looking near straight up or down)
        const float3 direction = float3_normalize(float3_subtract(target, position));
        if (fabsf(direction.y) < 0.9f) {
            const float3 above = float3_add(target, (float3) { 0, 0.1f, 0 });
            const float4 seen_above = camera_project_point(view_projection, above);
            if (seen_above.y <= seen.y) {
                below++;
            }
        }
    }
    zox_tst_check(!not_unit, "[%i] look rotations aren't unit length", not_unit);
    zox_tst_check(!behind, "[%i] targets are behind the camera looking at them", behind);
    zox_tst_check(worst_centre < zox_tst_camera_epsilon, "a target is off the view's centre by [%f]", worst_centre);
    zox_tst_check(!below, "[%i] points above the target are seen below it (upside down)", below);
}

// tst: camera_viewport_spawn
#define zox_tst_viewports 4
entity zox_tst_viewport_cameras[zox_tst_viewports];
byte zox_tst_viewport_stage = 0;
int zox_tst_viewport_frames = 0;
int2 zox_tst_viewport_window = { 0, 0 };
const int2 zox_tst_viewport_resized = { 801, 603 };

// checks the cameras are placed in their grid cells of a window size
void zox_tst_viewport_check_sizes(ecs* world, int2 window, const char* label) {
    for (int i = 0; i < zox_tst_viewports; i++) {
        const entity camera = zox_tst_viewport_cameras[i];
        const float4 cell = viewport_grid_cell(i, 2, 2);
        const int2 size = zox_getv(camera, PixelSize);
        const int2 position = zox_getv(camera, ScreenPosition);
        const int2 expected_size = screen_to_canvas_size(window, cell);
        const int2 expected_position = screen_to_canvas_position(window, cell);
        zox_tst_check(int2_equals(size, expected_size), "%s: viewport [%i] size [%ix%i], expected [%ix%i]",
            label, i, size.x, size.y, expected_size.x, expected_size.y);
        zox_tst_check(int2_equals(position, expected_position), "%s: viewport [%i] at [%ix%i], expected [%ix%i]",
            label, i, position.x, position.y, expected_position.x, expected_position.y);
        // projection matches its own aspect, not the window's
        const float4x4 projection = zox_getv(camera, ProjectionMatrix);
        const float aspect = expected_size.y ? expected_size.x / (float) expected_size.y : 0;
        const float projected_aspect = projection.x.x ? projection.y.y / projection.x.x : 0;
        zox_tst_check(fabsf(projected_aspect - aspect) < zox_tst_camera_epsilon,
            "%s: viewport [%i] projection aspect [%f], its size's [%f]", label, i, projected_aspect, aspect);
    }
}

byte zox_tst_camera_viewport_spawn_ready(ecs* world) {
    const entity app = main_app;
    if (!zox_valid(app) || !zox_has(app, WindowSize)) {
        zox_tst_waiting = "the app's window";
        return 0;
    }
    zox_tst_viewport_frames++;
    zox_tst_progress(zox_tst_viewport_stage * 1000 + zox_tst_viewport_frames);
    if (zox_tst_viewport_stage == 0) {
        zox_tst_waiting = "the main menu";
        if (zox_tst_viewport_frames < 10) {
            return 0;
        }
        zox_tst_viewport_window = zox_getv(app, WindowSize);
        for (int i = 0; i < zox_tst_viewports; i++) {
            zox_tst_viewport_cameras[i] = spawn_viewport_camera(
                world,
                app,
                (float3) { 0, 0, 4 + i },
                quaternion_identity,
                60,
                viewport_grid_cell(i, 2, 2));
        }
        zox_tst_viewport_stage = 1;
        zox_tst_viewport_frames = 0;
        return 0;
    }
    if (zox_tst_viewport_stage == 1) {
        zox_tst_waiting = "the viewport cameras' matrices";
        if (zox_tst_viewport_frames < 5) {
            return 0;
        }
        for (int i = 0; i < zox_tst_viewports; i++) {
            const entity camera = zox_tst_viewport_cameras[i];
            zox_tst_check(zox_valid(camera), "viewport camera [%i] wasn't spawned", i);
            if (!zox_valid(camera)) {
                return 1;
            }
            zox_tst_check(zox_is_linked(world, app, ViewportCamera, camera), "viewport camera [%i] isn't linked to the app", i);
            zox_tst_check(!zox_has(camera, RenderCamera),
                "viewport camera [%i] draws to a texture, not the screen", i);
            zox_tst_check(zox_has(camera, Camera3) && zox_has(camera, Perspective), "viewport camera [%i] isn't a 3D perspective camera", i);
            // its view projection put a point in front of it in the middle of its view
            const float4 seen = camera_project_point(zox_getv(camera, ViewProjectionMatrix), (float3) { 0, 0, 0 });
            zox_tst_check(seen.w > 0 && fabsf(seen.x) + fabsf(seen.y) < zox_tst_camera_epsilon,
                "viewport camera [%i] sees the point ahead at [%f %f] w [%f]", i, seen.x, seen.y, seen.w);
        }
        zox_tst_viewport_check_sizes(world, zox_tst_viewport_window, "spawned");
        // each camera's own far distance (CameraFarDistance), set on one
        for (int i = 0; i < zox_tst_viewports; i++) {
            const float far = zox_getv(zox_tst_viewport_cameras[i], CameraFarDistance);
            zox_tst_check(far == camera_far_distance, "viewport camera [%i] far distance [%f], the default is [%f]", i, far, camera_far_distance);
        }
        zox_setv(zox_tst_viewport_cameras[0], CameraFarDistance, 10);
        // resize the window, the way sdl's resize event does
        zox_set(app, WindowSize, { zox_tst_viewport_resized });
        zox_add(app, WindowSizeDirty);
        zox_tst_viewport_stage = 2;
        zox_tst_viewport_frames = 0;
        return 0;
    }
    if (zox_tst_viewport_stage == 2) {
        zox_tst_waiting = "the viewports to resize";
        if (zox_tst_viewport_frames < 5) {
            return 0;
        }
        zox_tst_viewport_check_sizes(world, zox_tst_viewport_resized, "resized");
        // the short sighted camera (at z 4, looking down -z) can't see 20 ahead, the others can
        for (int i = 0; i < zox_tst_viewports; i++) {
            const float3 ahead = { 0, 0, 4 + i - 20 };
            const float4 seen = camera_project_point(zox_getv(zox_tst_viewport_cameras[i], ViewProjectionMatrix), ahead);
            const byte in_range = seen.w > 0 && seen.z > -1 && seen.z < 1;
            zox_tst_check(in_range == (i != 0), "viewport camera [%i] (far [%.0f]) %s a point 20 ahead (depth [%f])",
                i, zox_getv(zox_tst_viewport_cameras[i], CameraFarDistance), in_range ? "sees" : "doesn't see", seen.z);
        }
        // the game camera draws to a texture, scaled by viewport_downscale
        const entity game_camera = main_cameras[0];
        if (zox_valid(game_camera) && zox_has(game_camera, RenderCamera)) {
            const int2 game_size = zox_getv(game_camera, PixelSize);
            const int2 expected = scale_viewport(zox_tst_viewport_resized);
            zox_tst_check(int2_equals(game_size, expected), "game camera resized to [%ix%i], expected [%ix%i]",
                game_size.x, game_size.y, expected.x, expected.y);
        }
        // put the window back, and remove the views
        zox_set(app, WindowSize, { zox_tst_viewport_window });
        zox_add(app, WindowSizeDirty);
        for (int i = 0; i < zox_tst_viewports; i++) {
            zox_delete(zox_tst_viewport_cameras[i]);
        }
        zox_tst_viewport_stage = 3;
        zox_tst_viewport_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the viewport cameras to be deleted";
    return zox_tst_viewport_frames >= 5;
}

void zox_tst_camera_viewport_spawn_check(ecs* world) {
    const entity app = main_app;
    int deleted = 0;
    for (int i = 0; i < zox_tst_viewports; i++) {
        deleted += !zox_valid(zox_tst_viewport_cameras[i]);
    }
    zox_tst_check(deleted == zox_tst_viewports, "only [%i] of [%i] viewport cameras were deleted", deleted, zox_tst_viewports);
    // the app's viewport links are only its own cameras again
    uint links = 0;
    while (zox_get_link_index(world, app, ViewportCamera, links)) {
        const entity camera = zox_get_link_index(world, app, ViewportCamera, links);
        for (int i = 0; i < zox_tst_viewports; i++) {
            zox_tst_check(camera != zox_tst_viewport_cameras[i], "the app still links deleted viewport camera [%i]", i);
        }
        links++;
    }
    zox_tst_check(links > 0, "the app lost its own viewport cameras");
    const int2 window = zox_getv(app, WindowSize);
    zox_tst_check(int2_equals(window, zox_tst_viewport_window), "window size wasn't put back [%ix%i]", window.x, window.y);
}

// tst: camera_orthographic, the orthographic projection: a unit high view, as wide as its aspect
void zox_tst_camera_orthographic_check(ecs* world) {
    (void) world;
    const int2 sizes[3] = { { 1920, 1080 }, { 600, 600 }, { 300, 900 } };
    for (int i = 0; i < 3; i++) {
        const int2 size = sizes[i];
        const float aspect = size.x / (float) size.y;
        const float4x4 projection = calculate_orthographic_matrix(size);
        const float4 corner = camera_project_point(projection, (float3) { 0.5f * aspect, 0.5f, 0 });
        const float4 other = camera_project_point(projection, (float3) { -0.5f * aspect, -0.5f, 0 });
        const float4 near = camera_project_point(projection, (float3) { 0, 0, 0.9f });
        const float4 far = camera_project_point(projection, (float3) { 0, 0, -0.9f });
        // no perspective: distance doesn't shrink things
        const float4 deep = camera_project_point(projection, (float3) { 0.25f, 0.25f, -0.9f });
        const float4 shallow = camera_project_point(projection, (float3) { 0.25f, 0.25f, 0.9f });
        zox_tst_check(fabsf(corner.x - 1) + fabsf(corner.y - 1) < zox_tst_camera_epsilon,
            "[%ix%i] the view's top right lands at [%f %f]", size.x, size.y, corner.x, corner.y);
        zox_tst_check(fabsf(other.x + 1) + fabsf(other.y + 1) < zox_tst_camera_epsilon,
            "[%ix%i] the view's bottom left lands at [%f %f]", size.x, size.y, other.x, other.y);
        zox_tst_check(near.z > -1 && near.z < 1 && far.z > -1 && far.z < 1 && near.z < far.z,
            "[%ix%i] depths out of order or range: near [%f] far [%f]", size.x, size.y, near.z, far.z);
        zox_tst_check(fabsf(deep.x - shallow.x) + fabsf(deep.y - shallow.y) < zox_tst_camera_epsilon,
            "[%ix%i] orthographic shrinks with distance", size.x, size.y);
    }
}

// checks a camera sits in a cell of the window (scaled down when it draws to a texture)
void zox_tst_camera_in_cell(ecs* world, entity camera, int2 window, float4 cell, const char* label) {
    if (!zox_valid(camera)) {
        zox_tst_check(0, "%s: camera missing", label);
        return;
    }
    const int2 full = screen_to_canvas_size(window, cell);
    const int2 expected_size = camera_viewport_size(world, camera, full);
    const int2 expected_position = screen_to_canvas_position(window, cell);
    const int2 size = zox_getv(camera, PixelSize);
    const int2 position = zox_getv(camera, ScreenPosition);
    zox_tst_check(int2_equals(size, expected_size), "%s [%s]: size [%ix%i], expected [%ix%i]",
        label, zox_getn(camera), size.x, size.y, expected_size.x, expected_size.y);
    zox_tst_check(int2_equals(position, expected_position), "%s [%s]: at [%ix%i], expected [%ix%i]",
        label, zox_getn(camera), position.x, position.y, expected_position.x, expected_position.y);
}

// resizes the app's window the way sdl's resize event does
void zox_tst_camera_resize_window(ecs* world, int2 size) {
    zox_set(main_app, WindowSize, { size });
    zox_add(main_app, WindowSizeDirty);
}

// tst: camera_split_screen, --splitscreen: two players side by side, each its own game and ui camera and canvas
//  - placed in their halves, follow a window resize; the menus, no game
byte zox_tst_splitscreen_stage = 0;
int zox_tst_splitscreen_frames = 0;
int2 zox_tst_splitscreen_window;

void zox_tst_camera_split_screen_setup() {
    is_split_screen = 1;
}

void zox_tst_splitscreen_check_cells(ecs* world, int2 window, const char* label) {
    for (int i = 0; i < 2; i++) {
        const float4 cell = viewport_grid_cell(i, 2, 1);
        zox_tst_camera_in_cell(world, main_cameras[i], window, cell, label);
        zox_tst_camera_in_cell(world, ui_cameras[i], window, cell, label);
        const entity canvas = zox_get_link(world, zox_players[i], CanvasLink);
        zox_tst_check(zox_valid(canvas), "%s: player [%i] has no canvas", label, i);
        if (zox_valid(canvas)) {
            const int2 size = zox_getv(canvas, LayoutSize);
            const int2 expected = screen_to_canvas_size(window, cell);
            zox_tst_check(int2_equals(size, expected), "%s: player [%i]'s canvas [%ix%i], expected [%ix%i]",
                label, i, size.x, size.y, expected.x, expected.y);
        }
    }
}

byte zox_tst_camera_split_screen_ready(ecs* world) {
    zox_tst_splitscreen_frames++;
    zox_tst_progress(zox_tst_splitscreen_stage * 1000 + zox_tst_splitscreen_frames);
    if (zox_tst_splitscreen_stage == 0) {
        zox_tst_waiting = "both players' cameras";
        if (zox_tst_splitscreen_frames < 10 || !zox_valid(main_app) || !zox_valid(main_cameras[0]) || !zox_valid(main_cameras[1])) {
            return 0;
        }
        zox_tst_splitscreen_window = zox_getv(main_app, WindowSize);
        zox_tst_check(main_cameras_count == 2, "[%i] main cameras for split screen", main_cameras_count);
        zox_tst_check(main_cameras[0] != main_cameras[1] && ui_cameras[0] != ui_cameras[1], "players share a camera");
        for (int i = 0; i < 2; i++) {
            zox_tst_check(zox_get_link(world, zox_players[i], CameraLink) == main_cameras[i],
                "player [%i]'s camera isn't its game camera", i);
            zox_tst_check(zox_is_linked(world, main_app, ViewportCamera, main_cameras[i]) &&
                zox_is_linked(world, main_app, ViewportCamera, ui_cameras[i]),
                "player [%i]'s cameras aren't the app's viewports", i);
        }
        zox_tst_splitscreen_check_cells(world, zox_tst_splitscreen_window, "spawned");
        zox_tst_camera_resize_window(world, (int2) { 1001, 601 });
        zox_tst_splitscreen_stage = 1;
        zox_tst_splitscreen_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the split screen to resize";
    if (zox_tst_splitscreen_stage == 1) {
        if (zox_tst_splitscreen_frames < 5) {
            return 0;
        }
        zox_tst_splitscreen_check_cells(world, (int2) { 1001, 601 }, "resized");
        zox_tst_camera_resize_window(world, zox_tst_splitscreen_window);
        zox_tst_splitscreen_stage = 2;
        zox_tst_splitscreen_frames = 0;
        return 0;
    }
    if (zox_tst_splitscreen_frames < 5) {
        return 0;
    }
    zox_tst_splitscreen_check_cells(world, zox_tst_splitscreen_window, "restored");
    return 1;
}

void zox_tst_camera_split_screen_check(ecs* world) {
    (void) world;
}

// tst: camera_downscale, viewport_downscale (the web build's 4): render cameras draw to a texture at a quarter,
//  cameras drawing to the screen keep its full size, both through a window resize
byte zox_tst_downscale_stage = 0;
int zox_tst_downscale_frames = 0;
int2 zox_tst_downscale_window;
entity zox_tst_downscale_camera = 0;

void zox_tst_camera_downscale_setup() {
    viewport_downscale = 4;
}

byte zox_tst_camera_downscale_ready(ecs* world) {
    zox_tst_downscale_frames++;
    zox_tst_progress(zox_tst_downscale_stage * 1000 + zox_tst_downscale_frames);
    const entity game_camera = main_cameras[0];
    if (zox_tst_downscale_stage == 0) {
        zox_tst_waiting = "the game camera";
        if (zox_tst_downscale_frames < 10 || !zox_valid(main_app) || !zox_valid(game_camera)) {
            return 0;
        }
        zox_tst_downscale_window = zox_getv(main_app, WindowSize);
        zox_tst_check(zox_has(game_camera, RenderCamera), "the game camera doesn't draw to a texture");
        const int2 size = zox_getv(game_camera, PixelSize);
        const int2 expected = int2_scale1(zox_tst_downscale_window, 0.25f);
        zox_tst_check(int2_equals(size, expected), "game camera [%ix%i] isn't a quarter of the window [%ix%i]",
            size.x, size.y, zox_tst_downscale_window.x, zox_tst_downscale_window.y);
        zox_tst_downscale_camera = spawn_viewport_camera(world, main_app, float3_zero, quaternion_identity, 60, single_screen_to_canvas);
        zox_tst_downscale_stage = 1;
        zox_tst_downscale_frames = 0;
        return 0;
    }
    if (zox_tst_downscale_stage == 1) {
        zox_tst_waiting = "the viewport camera";
        if (zox_tst_downscale_frames < 5) {
            return 0;
        }
        zox_tst_camera_in_cell(world, zox_tst_downscale_camera, zox_tst_downscale_window, single_screen_to_canvas, "spawned");
        const int2 size = zox_getv(zox_tst_downscale_camera, PixelSize);
        zox_tst_check(int2_equals(size, zox_tst_downscale_window), "a screen camera [%ix%i] isn't the window's size", size.x, size.y);
        zox_tst_camera_resize_window(world, (int2) { 800, 400 });
        zox_tst_downscale_stage = 2;
        zox_tst_downscale_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the cameras to resize";
    if (zox_tst_downscale_frames < 5) {
        return 0;
    }
    const int2 game_size = zox_getv(game_camera, PixelSize);
    const int2 screen_size = zox_getv(zox_tst_downscale_camera, PixelSize);
    zox_tst_check(int2_equals(game_size, ((int2) { 200, 100 })), "resized game camera [%ix%i], expected [200x100]", game_size.x, game_size.y);
    zox_tst_check(int2_equals(screen_size, ((int2) { 800, 400 })), "resized screen camera [%ix%i], expected [800x400]", screen_size.x, screen_size.y);
    zox_tst_camera_resize_window(world, zox_tst_downscale_window);
    zox_delete(zox_tst_downscale_camera);
    return 1;
}

void zox_tst_camera_downscale_check(ecs* world) {
    (void) world;
}

void zox_tsts_cameras() {
    zox_tst_add("camera_viewports", zox_tst_camera_ready_now, zox_tst_camera_viewports_check, 30);
    zox_tst_set_setup("camera_viewports", NULL, 1);
    zox_tst_add("camera_projection", zox_tst_camera_ready_now, zox_tst_camera_projection_check, 30);
    zox_tst_set_setup("camera_projection", NULL, 1);
    zox_tst_add("camera_look_at", zox_tst_camera_ready_now, zox_tst_camera_look_at_check, 30);
    zox_tst_set_setup("camera_look_at", NULL, 1);
    zox_tst_add("camera_viewport_spawn", zox_tst_camera_viewport_spawn_ready, zox_tst_camera_viewport_spawn_check, 30);
    zox_tst_set_setup("camera_viewport_spawn", NULL, 1);
    zox_tst_add("camera_orthographic", zox_tst_camera_ready_now, zox_tst_camera_orthographic_check, 30);
    zox_tst_set_setup("camera_orthographic", NULL, 1);
    zox_tst_add("camera_split_screen", zox_tst_camera_split_screen_ready, zox_tst_camera_split_screen_check, 30);
    zox_tst_set_setup("camera_split_screen", zox_tst_camera_split_screen_setup, 1);
    zox_tst_add("camera_downscale", zox_tst_camera_downscale_ready, zox_tst_camera_downscale_check, 30);
    zox_tst_set_setup("camera_downscale", zox_tst_camera_downscale_setup, 1);
}
