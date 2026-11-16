// tst: xr_sim_eyes, the XR simulator draws two eyes side by side (sim/_.c, fun/eye_render.c)
//  - on the menus with --xr-sim's setup, the test level (flat): the xr camera spawns, no screen camera
//  - screenshots the window (tst/xr_sim.ppm): each half drew the world, not just its clear colour,
//      and the halves differ a little (64mm apart), not a lot (the same view)
//  - stood above the ground looking level: the ground (coloured, its texture and reach vary) under the horizon,
//      the sky (grey) above, upright (the projection once flipped y: the ground drew on top, its faces inside out)
//  - run: bash bsh/tst.sh xr_sim_eyes
int zox_tst_xr_sim_frames = 0;
byte zox_tst_xr_sim_shot = 0;

void zox_tst_xr_sim_setup() {
    zox_tst_level_setup();
    zox_xr_sim = 1;
    zox_xr_active = 1;
    zox_huds3D = 1;
}

byte zox_tst_xr_sim_eyes_ready(ecs* world) {
    if (!zox_valid(xr_camera)) {
        zox_tst_waiting = "the xr camera";
        return 0;
    }
    zox_tst_xr_sim_frames++;
    zox_tst_waiting = "the world to draw";
    if (zox_tst_xr_sim_frames == 1) {
        remove(zox_tst_path("xr_sim.ppm"));
        // stood above the flat ground, looking level
        // placed as a game's start places it, the roam takes it up (fun/roam.c)
        zox_setv(xr_camera, Position3D, ((float3) { 0, 6, 0 }));
        zox_setv(xr_camera, Rotation3D, float4_identity);
        xr_sim_head_euler = float3_zero;
    }
    if (zox_tst_xr_sim_frames == 240) {
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("xr_sim.ppm"));
    }
    return zox_tst_xr_sim_frames > 240 && !zox_screenshot_request[0];
}

void zox_tst_xr_sim_eyes_check(ecs* world) {
    (void) world;
    zox_tst_check(!zox_valid(main_cameras[0]) || zox_has(main_cameras[0], XrCamera), "the main camera isn't the xr camera");
    int2 size;
    byte* pixels = zox_tst_load_ppm(zox_tst_path("xr_sim.ppm"), &size);
    zox_tst_check(pixels != NULL, "no screenshot tst/xr_sim.ppm");
    if (!pixels) {
        return;
    }
    const int half = size.x / 2;
    // how many of each half's pixels aren't the clear colour, and how far the halves differ
    int drawn[2] = { 0, 0 };
    double difference = 0;
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < half; x++) {
            for (int i = 0; i < 2; i++) {
                const byte* p = zox_tst_ppm_pixel(pixels, size, x + i * half, y);
                drawn[i] += abs(p[0] - xr_clear_color.r) + abs(p[1] - xr_clear_color.g) + abs(p[2] - xr_clear_color.b) > 12;
            }
            const byte* a = zox_tst_ppm_pixel(pixels, size, x, y);
            const byte* b = zox_tst_ppm_pixel(pixels, size, x + half, y);
            difference += abs(a[0] - b[0]) + abs(a[1] - b[1]) + abs(a[2] - b[2]);
        }
    }
    const float total = (float) half * size.y;
    difference /= total * 3;
    zox_log("[tst] [metric] xr_sim_eyes drawn [%.2f] [%.2f], halves differ by [%.1f] a channel", drawn[0] / total, drawn[1] / total, difference);
    zox_tst_check(drawn[0] > total * 0.3f && drawn[1] > total * 0.3f, "an eye drew little but its clear colour [%.2f] [%.2f]", drawn[0] / total, drawn[1] / total);
    // coloured pixels (the ground's, the sky's grey has none) in each half of the left eye, all below the horizon
    int coloured[2] = { 0, 0 };
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < half; x++) {
            const byte* p = zox_tst_ppm_pixel(pixels, size, x, y);
            const int saturation = int_max(p[0], int_max(p[1], p[2])) - int_min(p[0], int_min(p[1], p[2]));
            coloured[y >= size.y / 2] += saturation > 40;
        }
    }
    zox_log("[tst] xr_sim_eyes coloured pixels, top half [%i] bottom half [%i]", coloured[0], coloured[1]);
    zox_tst_check(coloured[1] > total * 0.01f && coloured[0] * 10 < coloured[1], "not upright: the ground (coloured) isn't under the horizon, top half [%i] bottom half [%i]", coloured[0], coloured[1]);
    zox_tst_check(difference > 0.01f, "the eyes drew the same image, no parallax");
    zox_tst_check(difference < 40, "the eyes drew different things [%.1f]", difference);
    free(pixels);
}

void zox_tsts_xr_sim(ecs* world) {
    zox_tst_add("xr_sim_eyes", zox_tst_xr_sim_eyes_ready, zox_tst_xr_sim_eyes_check, 60);
    zox_tst_set_setup("xr_sim_eyes", zox_tst_xr_sim_setup, 1);
}
