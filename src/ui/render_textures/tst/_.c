// tst: eyelids, a camera's lids close over its view (CameraLids, res/shaders/render_texture_rgb.frag)
//  - the player's game, its camera's view saved three times: lids open (0), part shut (0.7), shut (1)
//  - part shut: the lids reach further in the middle than at the sides (their curve): spots dark in the middle,
//    the same as open at the sides
//  - shut: dark everywhere the view shows (spots clear of the 2D ui drawn over it, its bars, minimap, text)
//  - run: bash bsh/tst.sh eyelids
extern char zox_screenshot_request[256];
byte zox_tst_lids_stage = 0;
int zox_tst_lids_frames = 0;
const float zox_tst_lids_values[3] = { 0, 0.7f, 1 };
const char* zox_tst_lids_files[3] = { "eyelids_open.ppm", "eyelids_half.ppm", "eyelids_shut.ppm" };

byte zox_tst_eyelids_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity camera = main_cameras[0];
    if (!zox_valid(camera) || !zox_has(camera, CameraLids)) {
        return 0;
    }
    zox_tst_lids_frames++;
    zox_tst_progress(zox_tst_lids_stage * 1000 + zox_tst_lids_frames);
    if (zox_screenshot_request[0]) {
        return 0;
    }
    if (zox_tst_lids_stage >= 3) {
        return 1;
    }
    // set, then a few frames to draw with it, then saved
    if (zox_tst_lids_frames == 1) {
        zox_setv(camera, CameraLids, zox_tst_lids_values[zox_tst_lids_stage]);
        return 0;
    }
    if (zox_tst_lids_frames < 10) {
        return 0;
    }
    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(zox_tst_lids_files[zox_tst_lids_stage]));
    zox_tst_lids_stage++;
    zox_tst_lids_frames = 0;
    return 0;
}

// the mean brightness of a pixel's 5 x 5 square (0 to 765), at x, y of the image (0 to 1, from the top)
static inline int zox_tst_lids_at(const byte* pixels, int2 size, float x, float y) {
    const int cx = (int) (x * (size.x - 1)), cy = (int) (y * (size.y - 1));
    int sum = 0, count = 0;
    for (int i = -2; i <= 2; i++) for (int j = -2; j <= 2; j++) {
        const int px = int_max(0, int_min(size.x - 1, cx + i)), py = int_max(0, int_min(size.y - 1, cy + j));
        const byte* p = zox_tst_ppm_pixel(pixels, size, px, py);
        sum += p[0] + p[1] + p[2];
        count++;
    }
    return sum / count;
}

void zox_tst_eyelids_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_lids_stage == 3, "it stopped at stage [%i]", zox_tst_lids_stage);
    int2 sizes[3];
    byte* images[3];
    for (byte i = 0; i < 3; i++) {
        images[i] = zox_tst_load_ppm(zox_tst_path(zox_tst_lids_files[i]), &sizes[i]);
        zox_tst_check(images[i], "[%s] wasn't saved", zox_tst_lids_files[i]);
    }
    if (images[0] && images[1] && images[2]) {
        const byte* open = images[0];
        const byte* half = images[1];
        const byte* shut = images[2];
        const int2 size = sizes[0];
        const int dark = 30;
        // spots where the 3D view shows: under the bars and minimap (0.46), over the action bar (0.75), not the
        //  crosshair or the quest's text (0.55 to 0.63 from the top, from x 0.23)
        // part shut (0.7): its lids 0.6 * 0.7 and 0.4 * 0.7 of the height at the sides, 1.35 times that in the middle
        //  - x 0.4: the top lid to 0.56, the bottom from 0.63; x 0.02 (the side): the top to 0.43, the bottom from 0.71
        const int mid_top = zox_tst_lids_at(half, size, 0.4f, 0.5f), mid_bottom = zox_tst_lids_at(half, size, 0.4f, 0.68f);
        const int side_top = zox_tst_lids_at(half, size, 0.02f, 0.5f), side_bottom = zox_tst_lids_at(half, size, 0.02f, 0.68f);
        const int side_top_open = zox_tst_lids_at(open, size, 0.02f, 0.5f), side_bottom_open = zox_tst_lids_at(open, size, 0.02f, 0.68f);
        zox_log("[tst] part shut: middle [%i %i] sides [%i %i] (open [%i %i])", mid_top, mid_bottom, side_top, side_bottom,
            side_top_open, side_bottom_open);
        zox_tst_check(mid_top < dark && mid_bottom < dark, "part shut, the middle isn't under the lids [%i %i]", mid_top, mid_bottom);
        zox_tst_check(abs(side_top - side_top_open) < 40 && abs(side_bottom - side_bottom_open) < 40,
            "part shut, the sides changed [%i %i] from open [%i %i]: the lids aren't curved", side_top, side_bottom, side_top_open, side_bottom_open);
        zox_tst_check(side_top_open >= dark && side_bottom_open >= dark, "the open view is dark at the sides [%i %i], nothing to compare",
            side_top_open, side_bottom_open);
        // shut: all dark
        const float2 spots[5] = { { 0.15f, 0.5f }, { 0.8f, 0.5f }, { 0.1f, 0.68f }, { 0.6f, 0.7f }, { 0.3f, 0.45f } };
        for (byte i = 0; i < 5; i++) {
            const int v = zox_tst_lids_at(shut, size, spots[i].x, spots[i].y);
            const int v_open = zox_tst_lids_at(open, size, spots[i].x, spots[i].y);
            zox_tst_check(v < dark, "shut, [%.2f %.2f] is lit [%i] (open [%i])", spots[i].x, spots[i].y, v, v_open);
        }
    }
    for (byte i = 0; i < 3; i++) {
        free(images[i]);
    }
}

void zox_tsts_render_textures(ecs* world) {
    zox_tst_add("eyelids", zox_tst_eyelids_ready, zox_tst_eyelids_check, 120);
}
