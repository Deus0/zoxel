// tst: eyelids, a camera's lids close over its view (CameraLids, res/shaders/render_texture_rgb.frag)
//  - on the menus, no realm or player: the menu's camera (every 3D camera has lids) in a void, the skybox disabled,
//    a light grey clear, no vignette or blur, so its whole view is evenly lit (the menu's cube stays, lit too)
//  - its view saved three times: lids open (0), part shut (0.7), shut (1)
//  - part shut: an eye's shape, the lids reach further at the sides than in the middle (their curve): a spot
//    between the lids in the middle is lit, the same height at the side is shut; above and below the middle's
//    opening dark (the menu's title band, 0.44 to 0.56, isn't lidded: spots clear of it)
//  - shut: dark everywhere the view shows (spots clear of the title band, 0.44 to 0.56 from the top)
//  - run: bash bsh/tst.sh eyelids
extern char zox_screenshot_request[256];
byte zox_tst_lids_stage = 0;
int zox_tst_lids_frames = 0;
byte zox_tst_lids_void = 0;
const float zox_tst_lids_values[3] = { 0, 0.7f, 1 };
const char* zox_tst_lids_files[3] = { "eyelids_open.ppm", "eyelids_half.ppm", "eyelids_shut.ppm" };

// the camera in a void: no skybox (Disabled, out of every query), a light clear colour, nothing over its view
void zox_tst_lids_make_void(ecs* world, entity camera) {
    entity skyboxes[8];
    int count = 0;
    iter it = zox_each(world, RenderAtCamera);
    while (zox_each_next(it)) {
        for (int j = 0; j < it.count && count < 8; j++) {
            skyboxes[count++] = it.entities[j];
        }
    }
    for (int i = 0; i < count; i++) {
        zox_add(skyboxes[i], Disabled);
    }
    zox_setv(camera, Color, ((color) { 200, 200, 200, 255 }));
    zox_setv(camera, CameraVignette, 0);
    zox_setv(camera, CameraBlur, 0);
}

byte zox_tst_eyelids_ready(ecs* world) {
    const entity camera = main_cameras[0];
    if (!zox_valid(camera) || !zox_has(camera, CameraLids)) {
        zox_tst_waiting = "the menu's camera";
        return 0;
    }
    if (!zox_tst_lids_void) {
        zox_tst_lids_make_void(world, camera);
        zox_tst_lids_void = 1;
    }
    // the screen's fader gone first (it fades out over time: a slower frame rate took the open view under it)
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
    zox_tst_waiting = "the lids' screenshots";
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
        // part shut (0.7), the shader's lids from the top 0.6 * 0.7 and the bottom 0.4 * 0.7 of the height, times
        //  1 + 0.8 u * u (u -1 to 1 across), from the top of the image:
        //  - x 0.4: the top lid to 0.43, the bottom from 0.71: open between; x 0.05 (the side): the top to 0.69, the
        //    bottom from 0.54, they meet: shut
        //  - y 0.62 is open in the middle, shut at the side: the curve; y 0.40 and 0.75 are under the middle's lids
        const int mid_top = zox_tst_lids_at(half, size, 0.4f, 0.40f), mid_bottom = zox_tst_lids_at(half, size, 0.4f, 0.75f);
        const int mid_gap = zox_tst_lids_at(half, size, 0.4f, 0.62f), mid_gap_open = zox_tst_lids_at(open, size, 0.4f, 0.62f);
        const int side = zox_tst_lids_at(half, size, 0.05f, 0.62f), side_open = zox_tst_lids_at(open, size, 0.05f, 0.62f);
        zox_log("[tst] part shut: middle [%i %i %i] (open [%i]) side [%i] (open [%i])", mid_top, mid_gap, mid_bottom,
            mid_gap_open, side, side_open);
        zox_tst_check(mid_gap_open >= dark && side_open >= dark, "the open view is dark [%i %i], nothing to compare",
            mid_gap_open, side_open);
        zox_tst_check(mid_top < dark && mid_bottom < dark, "part shut, the middle isn't under the lids [%i %i]", mid_top, mid_bottom);
        // lit, not the same as open: the menu's model turns behind it between the views
        zox_tst_check(mid_gap >= dark, "part shut, the middle's opening is dark [%i] (open [%i])", mid_gap, mid_gap_open);
        zox_tst_check(side < dark, "part shut, the side is open [%i]: the lids aren't an eye's shape", side);
        // shut: all dark, where open was lit
        const float2 spots[5] = { { 0.15f, 0.3f }, { 0.8f, 0.3f }, { 0.1f, 0.68f }, { 0.6f, 0.7f }, { 0.3f, 0.2f } };
        for (byte i = 0; i < 5; i++) {
            const int v = zox_tst_lids_at(shut, size, spots[i].x, spots[i].y);
            const int v_open = zox_tst_lids_at(open, size, spots[i].x, spots[i].y);
            zox_tst_check(v_open >= dark, "open, [%.2f %.2f] is dark [%i], nothing to shut", spots[i].x, spots[i].y, v_open);
            zox_tst_check(v < dark, "shut, [%.2f %.2f] is lit [%i] (open [%i])", spots[i].x, spots[i].y, v, v_open);
        }
    }
    for (byte i = 0; i < 3; i++) {
        free(images[i]);
    }
}

#include "hdr.c"

void zox_tsts_render_textures(ecs* world) {
    zox_tst_add("eyelids", zox_tst_eyelids_ready, zox_tst_eyelids_check, 30);
    zox_tst_set_setup("eyelids", NULL, 1);
    zox_tsts_render_textures_hdr(world);
}
