// tst: hdr_target and hdr_bloom, a render camera's half float target keeps colours past 1 for the post pass' tonemap
//  and bloom (doc/plans/hdr.md, res/shaders/render_texture_rgb.frag tone() and bloom_light())
//  - on the menus: the menu's camera in a void (zox_tst_lids_make_void), a grey cube (basic3, 200) a unit in front,
//    500 up away from the menu's model, read under the title's band; four views each, the settings and the cube's brightness changed between them (a
//    change to the target's format or mips remakes it: zox_set_post_setting)
//  - hdr_target: clamp and the soft tonemap at brightness 1, filmic at 1.5 and 3
//    - below the knee (200 is 0.78, under 0.8) the soft tonemap leaves it as clamp does
//    - past 1: 3 tonemaps brighter than 1.5 (both past 1 after the fog's mix). A byte target clamps both at 1
//      first: the same
//  - hdr_bloom: bloom off then on, the cube at 1 and at 3, read off the cube just below it (a dark background)
//    - off: the cube's brightness doesn't reach it; on: the brighter cube glows onto it
//  - run: bash bsh/tst.sh hdr_target, bash bsh/tst.sh hdr_bloom
typedef struct {
    byte tonemap;
    byte bloom;
    float brightness;
    const char* file;
} zox_tst_hdr_view;

const zox_tst_hdr_view zox_tst_hdr_target_views[4] = {
    { 0, 0, 1, "hdr_clamp_1.ppm" },
    { 1, 0, 1, "hdr_soft_1.ppm" },
    { 2, 0, 1.5f, "hdr_filmic_15.ppm" },
    { 2, 0, 3, "hdr_filmic_3.ppm" } };
const zox_tst_hdr_view zox_tst_hdr_bloom_views[4] = {
    { 0, 0, 1, "bloom_off_1.ppm" },
    { 0, 0, 3, "bloom_off_3.ppm" },
    { 0, 1, 1, "bloom_on_1.ppm" },
    { 0, 1, 3, "bloom_on_3.ppm" } };
byte zox_tst_hdr_stage = 0;
int zox_tst_hdr_frames = 0;
entity zox_tst_hdr_cube = 0;
byte zox_tst_hdr_viewport = 0;      // seen through a viewport camera over the whole screen (hdr_viewport_bloom)

// background: the clear's grey (the void's is 200)
byte zox_tst_hdr_views_ready(ecs* world, const zox_tst_hdr_view* views, byte background) {
    const entity camera = main_cameras[0];
    if (!zox_valid(camera)) {
        zox_tst_waiting = "the menu's camera";
        return 0;
    }
    if (!zox_tst_hdr_cube) {
        zox_tst_lids_make_void(world, camera);
        zox_setv(camera, Color, ((color) { background, background, background, 255 }));
        // up in an empty sky, away from the menu's model: the background round the cube plain grey
        float3 eye = zox_getv(camera, Position3D);
        eye.y += 500;
        zox_setv(camera, Position3D, eye);
        const float3 position = move_along_direction(eye, zox_getv(camera, Rotation3D), -1);
        zox_tst_hdr_cube = spawn_cube(world, prefab_cube, position, 0.5f);
        zox_setv(zox_tst_hdr_cube, Color, ((color) { 200, 200, 200, 255 }));
        if (zox_tst_hdr_viewport) {
            // its own view in its own texture over the game view's, the background its; the game view's light grey
            const entity viewport = spawn_viewport_camera(world, main_app, eye, zox_getv(camera, Rotation3D),
                zox_getv(camera, FieldOfView), single_screen_to_canvas);
            zox_setv(viewport, Color, ((color) { background, background, background, 255 }));
            zox_setv(camera, Color, ((color) { 200, 200, 200, 255 }));
        }
    }
    zox_tst_waiting = "the screen fader to fade out";
    byte fading = 0;
    iter it = zox_each(world, CanvasOverlay);
    while (zox_each_next(it)) {
        for (int j = 0; j < it.count; j++) {
            const entity overlay = it.entities[j];
            fading |= !zox_is_prefab(overlay) && zox_has(overlay, Alpha) && zox_getv(overlay, Alpha) > 0.01f;
        }
    }
    if (fading) {
        return 0;
    }
    zox_tst_waiting = "the cube's views";
    zox_tst_hdr_frames++;
    zox_tst_progress(zox_tst_hdr_stage * 1000 + zox_tst_hdr_frames);
    if (zox_screenshot_request[0]) {
        return 0;
    }
    if (zox_tst_hdr_stage >= 4) {
        return 1;
    }
    const zox_tst_hdr_view* view = &views[zox_tst_hdr_stage];
    if (zox_tst_hdr_frames == 1) {
        zox_set_tonemap(world, view->tonemap);
        zox_set_bloom(world, view->bloom);
        zox_setv(zox_tst_hdr_cube, Brightness, view->brightness);
        return 0;
    }
    if (zox_tst_hdr_frames < 10) {
        return 0;
    }
    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(view->file));
    zox_tst_hdr_stage++;
    zox_tst_hdr_frames = 0;
    return 0;
}

// each view's brightness at x, y (0 to 765, zox_tst_lids_at), 0 if any wasn't saved
byte zox_tst_hdr_reads(const zox_tst_hdr_view* views, float x, float y, int* v) {
    zox_tst_check(zox_tst_hdr_stage == 4, "it stopped at stage [%i]", zox_tst_hdr_stage);
    byte loaded = 1;
    for (byte i = 0; i < 4; i++) {
        int2 size;
        byte* image = zox_tst_load_ppm(zox_tst_path(views[i].file), &size);
        zox_tst_check(image, "[%s] wasn't saved", views[i].file);
        if (!image) {
            loaded = 0;
            continue;
        }
        v[i] = zox_tst_lids_at(image, size, x, y);
        free(image);
    }
    return loaded;
}

byte zox_tst_hdr_ready(ecs* world) {
    return zox_tst_hdr_views_ready(world, zox_tst_hdr_target_views, 200);
}

void zox_tst_hdr_check(ecs* world) {
    (void) world;
    int v[4] = { 0 };
    if (!zox_tst_hdr_reads(zox_tst_hdr_target_views, 0.5f, 0.62f, v)) {   // on the cube, under the title's band
        return;
    }
    zox_log("[tst] hdr: clamp 1 [%i] soft 1 [%i] filmic 1.5 [%i] filmic 3 [%i]", v[0], v[1], v[2], v[3]);
    zox_tst_check(v[0] >= 300, "the cube isn't lit [%i]", v[0]);
    zox_tst_check(abs(v[1] - v[0]) <= 6, "below the knee the soft tonemap changed it [%i] from [%i]", v[1], v[0]);
    zox_tst_check(v[3] - v[2] >= 30, "brightness 3 isn't brighter than 1.5 [%i %i]: the target clamps at 1", v[3], v[2]);
}

byte zox_tst_bloom_ready(ecs* world) {
    return zox_tst_hdr_views_ready(world, zox_tst_hdr_bloom_views, 40);   // dark: the glow shows
}

void zox_tst_bloom_check(ecs* world) {
    (void) world;
    int v[4] = { 0 };
    if (!zox_tst_hdr_reads(zox_tst_hdr_bloom_views, 0.5f, 0.70f, v)) {    // the background, just below the cube
        return;
    }
    zox_log("[tst] bloom: off 1 [%i] off 3 [%i] on 1 [%i] on 3 [%i]", v[0], v[1], v[2], v[3]);
    zox_tst_check(abs(v[1] - v[0]) <= 4, "bloom off, the cube's brightness reached the background [%i %i]", v[0], v[1]);
    zox_tst_check(v[3] - v[2] >= 10, "bloom on, the brighter cube didn't glow onto the background [%i %i]", v[2], v[3]);
}

// tst: hdr_viewport_bloom, hdr_bloom through a viewport camera (spawn_viewport_camera, doc/plans/hdr.md A5): its
//  screen texture drawn through the post pass, bloom and all; its background dark, the game view's under it grey
byte zox_tst_viewport_bloom_ready(ecs* world) {
    zox_tst_hdr_viewport = 1;
    return zox_tst_hdr_views_ready(world, zox_tst_hdr_bloom_views, 40);
}

void zox_tst_viewport_bloom_check(ecs* world) {
    int corner[4] = { 0 };
    if (!zox_tst_hdr_reads(zox_tst_hdr_bloom_views, 0.1f, 0.9f, corner)) {
        return;
    }
    zox_log("[tst] viewport bloom: its corner [%i] (its background ~120 of 765, the game view's ~600)", corner[0]);
    zox_tst_check(corner[0] < 300, "the viewport camera's view isn't on top: its corner [%i] is the game view's grey", corner[0]);
    zox_tst_bloom_check(world);
}

void zox_tsts_render_textures_hdr(ecs* world) {
    zox_tst_add("hdr_target", zox_tst_hdr_ready, zox_tst_hdr_check, 30);
    zox_tst_set_setup("hdr_target", NULL, 1);
    zox_tst_add("hdr_bloom", zox_tst_bloom_ready, zox_tst_bloom_check, 30);
    zox_tst_set_setup("hdr_bloom", NULL, 1);
    zox_tst_add("hdr_viewport_bloom", zox_tst_viewport_bloom_ready, zox_tst_viewport_bloom_check, 30);
    zox_tst_set_setup("hdr_viewport_bloom", NULL, 1);
}
