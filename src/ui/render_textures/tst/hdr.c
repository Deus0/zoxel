// tst: hdr_target, a render camera's half float target keeps colours past 1 for the post pass' tonemap
//  (doc/plans/hdr.md, res/shaders/render_texture_rgb.frag tone())
//  - on the menus: the menu's camera in a void (zox_tst_lids_make_void), a grey cube (basic3, 200) a unit in front,
//    read under the title's band
//  - four views of it: clamp and the soft tonemap at brightness 1, filmic at 1.5 and 3
//  - below the knee (200 is 0.78, under 0.8) the soft tonemap leaves it as clamp does
//  - past 1: 3 tonemaps brighter than 1.5 (both past 1 after the fog's mix). A byte target clamps both at 1
//    first: the same. Clamp draws into bytes, the tonemaps into half floats: the views' frames after a switch
//    remake the target (zox_set_tonemap)
//  - run: bash bsh/tst.sh hdr_target
byte zox_tst_hdr_stage = 0;
int zox_tst_hdr_frames = 0;
entity zox_tst_hdr_cube = 0;
const byte zox_tst_hdr_modes[4] = { 0, 1, 2, 2 };
const float zox_tst_hdr_brightness[4] = { 1, 1, 1.5f, 3 };
const char* zox_tst_hdr_files[4] = { "hdr_clamp_1.ppm", "hdr_soft_1.ppm", "hdr_filmic_15.ppm", "hdr_filmic_3.ppm" };

byte zox_tst_hdr_ready(ecs* world) {
    const entity camera = main_cameras[0];
    if (!zox_valid(camera)) {
        zox_tst_waiting = "the menu's camera";
        return 0;
    }
    if (!zox_tst_hdr_cube) {
        zox_tst_lids_make_void(world, camera);
        const float3 position = move_along_direction(zox_getv(camera, Position3D), zox_getv(camera, Rotation3D), -1);
        zox_tst_hdr_cube = spawn_cube(world, prefab_cube, position, 0.5f);
        zox_setv(zox_tst_hdr_cube, Color, ((color) { 200, 200, 200, 255 }));
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
    if (zox_tst_hdr_frames == 1) {
        zox_set_tonemap(world, zox_tst_hdr_modes[zox_tst_hdr_stage]);
        zox_setv(zox_tst_hdr_cube, Brightness, zox_tst_hdr_brightness[zox_tst_hdr_stage]);
        return 0;
    }
    if (zox_tst_hdr_frames < 10) {
        return 0;
    }
    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(zox_tst_hdr_files[zox_tst_hdr_stage]));
    zox_tst_hdr_stage++;
    zox_tst_hdr_frames = 0;
    return 0;
}

void zox_tst_hdr_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_hdr_stage == 4, "it stopped at stage [%i]", zox_tst_hdr_stage);
    int v[4] = { 0 };
    byte loaded = 1;
    for (byte i = 0; i < 4; i++) {
        int2 size;
        byte* image = zox_tst_load_ppm(zox_tst_path(zox_tst_hdr_files[i]), &size);
        zox_tst_check(image, "[%s] wasn't saved", zox_tst_hdr_files[i]);
        if (!image) {
            loaded = 0;
            continue;
        }
        v[i] = zox_tst_lids_at(image, size, 0.5f, 0.62f);   // on the cube, under the menu title's band
        free(image);
    }
    if (!loaded) {
        return;
    }
    zox_log("[tst] hdr: clamp 1 [%i] soft 1 [%i] filmic 1.5 [%i] filmic 3 [%i]", v[0], v[1], v[2], v[3]);
    zox_tst_check(v[0] >= 300, "the cube isn't lit [%i]", v[0]);
    zox_tst_check(abs(v[1] - v[0]) <= 6, "below the knee the soft tonemap changed it [%i] from [%i]", v[1], v[0]);
    zox_tst_check(v[3] - v[2] >= 30, "brightness 3 isn't brighter than 1.5 [%i %i]: the target clamps at 1", v[3], v[2]);
}

void zox_tsts_render_textures_hdr(ecs* world) {
    zox_tst_add("hdr_target", zox_tst_hdr_ready, zox_tst_hdr_check, 30);
    zox_tst_set_setup("hdr_target", NULL, 1);
}
