// tst: zeland_player_views, the game's own view of its character: four screenshots from the orbit camera round it
//  - plays, the character standing at its start; the camera turned 0, 90, 180 and 270 degrees round it (Euler y),
//    at the game's pitch and distance, each taken once the camera has eased there (still against the character for 3
//    frames: a character on a slope creeps, doc/todo.md Bugs)
//  - the game's fog and light, no hud
//  - checks: each saved, not one colour, the character (its orange capsule) in it
//  - run: bash bsh/tools/poly_terrain.sh --player (joins them into tst/poly/player_views.png)
const char* zox_tst_zpv_files[4] = { "player_view_1.ppm", "player_view_2.ppm", "player_view_3.ppm", "player_view_4.ppm" };
int zox_tst_zpv_view = 0;
byte zox_tst_zpv_turned = 0;
int zox_tst_zpv_still = 0;
float3 zox_tst_zpv_last;

byte zox_tst_zeland_player_views_ready(ecs* world) {
    nohud = 1;
    if (!zox_tst_zeland_boot_ready(world)) {
        return 0;
    }
    const entity character = zox_get_link(world, zox_players[0], CharacterLink);
    const entity camera = zox_get_link(world, zox_players[0], CameraLink);
    zox_tst_waiting = "the character on the ground, the orbit camera on it";
    if (!zox_valid(character) || zox_getv(camera, CameraState) != zox_camera_state_third_person ||
        !zox_getv(character, Grounded) || zox_screenshot_request[0]) {
        return 0;
    }
    if (zox_tst_zpv_view == 4) {
        return 1;
    }
    zox_tst_waiting = "the camera to ease round, then its screenshot";
    if (!zox_tst_zpv_turned) {
        float3 euler = zox_getv(camera, Euler);
        euler.y = zox_tst_zpv_view * M_PI / 2;
        zox_setv(camera, Euler, euler);
        zox_tst_zpv_turned = 1;
        zox_tst_zpv_still = 0;
            zox_tst_zpv_last = float3_sub(zox_getv(camera, Position3D), zox_getv(character, Position3D));
        return 0;
    }
    const float3 position = float3_sub(zox_getv(camera, Position3D), zox_getv(character, Position3D));
    zox_tst_zpv_still = float3_length(float3_sub(position, zox_tst_zpv_last)) < 0.005f ? zox_tst_zpv_still + 1 : 0;
    zox_tst_zpv_last = position;
    if (zox_tst_zpv_still < 3) {
        return 0;
    }
    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(zox_tst_zpv_files[zox_tst_zpv_view]));
    zox_tst_zpv_view++;
    zox_tst_zpv_turned = 0;
    return 0;
}

void zox_tst_zeland_player_views_check(ecs* world) {
    (void) world;
    for (int i = 0; i < 4; i++) {
        int2 size;
        byte* pixels = zox_tst_load_ppm(zox_tst_path(zox_tst_zpv_files[i]), &size);
        zox_tst_check(pixels, "[%s] wasn't saved", zox_tst_zpv_files[i]);
        if (!pixels) {
            continue;
        }
        byte varied = 0;
        int orange = 0;
        const size_t count = (size_t) size.x * size.y;
        for (size_t p = 1; p < count; p++) {
            const byte* rgb = pixels + p * 3;
            varied |= memcmp(pixels, rgb, 3) != 0;
            orange += rgb[0] > 120 && rgb[0] > rgb[1] + 40 && rgb[1] > rgb[2] + 15;
        }
        zox_tst_check(varied, "[%s] is one colour", zox_tst_zpv_files[i]);
        zox_tst_check(orange > 50, "[%s] doesn't show the character ([%i] orange pixels)", zox_tst_zpv_files[i], orange);
        free(pixels);
    }
}
