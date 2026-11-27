extern byte nohud;
extern char zox_screenshot_request[256];

static const char* zox_tst_poly_view_files[] = {
    "poly_view_1.ppm", "poly_view_2.ppm", "poly_view_3.ppm", "poly_view_4.ppm"
};
enum { zox_tst_poly_view_count = sizeof(zox_tst_poly_view_files) / sizeof(zox_tst_poly_view_files[0]) };
static int zox_tst_poly_view;
static double zox_tst_poly_view_time;
static byte zox_tst_poly_view_positioned;

static void zox_tst_poly_position_camera(ecs* world, entity camera, int view) {
    const float yaw = (45 + 90 * view) * degreesToRadians;
    // as far out as the island is wide, up at 45 degrees: its size, whatever the settings
    const float out = poly_terrain_size.x * 0.78f;
    const float3 position = { out * sinf(yaw), out * 0.55f, out * cosf(yaw) };
    const float4 rotation = camera_look_at(position, (float3) { 0, poly_terrain_height * 0.25f, 0 });
    zox_remove(camera, Roaming);
    zox_remove(camera, EternalRotation);
    zox_remove(camera, EulerOverride);
    zox_add(camera, DisableParentTransform);
    zox_setv(camera, FieldOfView, 60);
    zox_setv(camera, CameraVignette, 0);
    zox_setv(camera, CameraBlur, 0);
    zox_setv(camera, Position3D, position);
    zox_setv(camera, Euler, quaternion_to_euler(rotation));
    zox_setv(camera, Rotation3D, rotation);
}

static byte zox_tst_poly_views_ready(ecs* world) {
    nohud = 1;
    fog_density = 0;
    const char* seed = getenv("zox_tst_seed");
    if (!zox_tst_poly_uploaded(world, seed ? atoll(seed) : 666)) {
        return 0;
    }
    zox_tst_waiting = "the menu camera and four terrain screenshots";
    const entity camera = main_cameras[0];
    if (!zox_valid(camera) || zox_screenshot_request[0]) {
        return 0;
    }
    if (zox_tst_poly_view == zox_tst_poly_view_count) {
        return 1;
    }
    if (!zox_tst_poly_view_positioned) {
        zox_tst_poly_position_camera(world, camera, zox_tst_poly_view);
        zox_tst_poly_view_time = zox_current_time;
        zox_tst_poly_view_positioned = 1;
        return 0;
    }
    if (zox_current_time - zox_tst_poly_view_time >= 1) {
        const char* name = zox_tst_poly_view_files[zox_tst_poly_view++];
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(name));
        zox_tst_poly_view_positioned = 0;
    }
    return 0;
}

static void zox_tst_poly_views_check(ecs* world) {
    (void) world;
    for (int i = 0; i < zox_tst_poly_view_count; i++) {
        const char* name = zox_tst_poly_view_files[i];
        int2 size;
        byte* pixels = zox_tst_load_ppm(zox_tst_path(name), &size);
        zox_tst_check(pixels, "[%s] wasn't saved", name);
        if (!pixels) {
            continue;
        }
        byte varied = 0, grass = 0;
        const size_t count = (size_t) size.x * size.y;
        for (size_t p = 1; p < count && !(varied && grass); p++) {
            const byte* rgb = pixels + p * 3;
            varied |= memcmp(pixels, rgb, 3) != 0;
            grass |= rgb[1] > rgb[0] + 15 && rgb[1] > rgb[2] + 15;
        }
        zox_tst_check(varied, "[%s] is one colour", name);
        zox_tst_check(grass, "[%s] has no visible grass", name);
        free(pixels);
    }
}
