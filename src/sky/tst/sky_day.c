// tst: sky_day, the sky through a day (res/shaders/skybox.frag, doc/plans/day_night.md): the player lifted, its body
//  hidden, the realm's time set (paused) to dawn, noon, dusk and midnight, the camera looking at the sun (up, at
//  midnight); a screenshot each (tst/sky_day/sky_<time>.ppm) to look at
//  - the middle of the screen (the sun) is bright in the day, the realm's sunset colour at dawn and dusk (from its
//    seed: its strongest and weakest channels); at midnight the sky is dark
//  - and at dusk looking at the horizon under the sun (sky_horizon.ppm): the far terrain fades into the sky's glow
//  - run: bash bsh/tst.sh sky_day
extern char zox_screenshot_request[256];
#define zox_tst_sky_day_times 5
const float zox_tst_sky_day_of[zox_tst_sky_day_times] = { 0.27f, 0.5f, 0.73f, 0, 0.74f };
const char* zox_tst_sky_day_names[zox_tst_sky_day_times] = { "dawn", "noon", "dusk", "midnight", "horizon" };
float3 zox_tst_sky_day_sunset;
float3 zox_tst_sky_day_middle[zox_tst_sky_day_times];     // the middle's colour, 0 to 255
float zox_tst_sky_day_mean[zox_tst_sky_day_times];        // the whole sky's brightness
int zox_tst_sky_day_index = -1;
int zox_tst_sky_day_frame = 0;
float zox_tst_sky_day_ground = 0;

// a screenshot's middle square's mean colour, and its whole mean brightness
byte zox_tst_sky_day_read(const char* path, float3* middle, float* mean) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        return 0;
    }
    int width = 0, height = 0, max = 0;
    const int read = fscanf(file, "P6 %d %d %d", &width, &height, &max);
    fgetc(file);
    if (read != 3 || width <= 0 || height <= 0) {
        fclose(file);
        return 0;
    }
    double sum[3] = { 0, 0, 0 }, all = 0;
    int count = 0;
    byte rgb[3];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (fread(rgb, 1, 3, file) != 3) {
                break;
            }
            all += (rgb[0] + rgb[1] + rgb[2]) / 3.0;
            if (abs(x - width / 2) < width / 40 && abs(y - height / 2) < height / 40) {
                sum[0] += rgb[0];
                sum[1] += rgb[1];
                sum[2] += rgb[2];
                count++;
            }
        }
    }
    fclose(file);
    *middle = count ? (float3) { sum[0] / count, sum[1] / count, sum[2] / count } : float3_zero;
    *mean = all / (width * height);
    return 1;
}

// the head's camera looking along a world direction
void zox_tst_sky_day_look(ecs* world, entity character, float3 direction) {
    const entity skeleton = get_skeleton(world, character);
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    const entity camera = zox_valid(head) ? zox_get_child_by_id(world, head, zox_id(Camera)) : 0;
    if (!zox_valid(camera)) {
        return;
    }
    const float pitch = asinf(clampf(direction.y, -1, 1));
    const float yaw = atan2f(direction.x, direction.z) + (is_camera_positive_z ? M_PI : 0);
    const float4 look = euler_to_quaternion((float3) { pitch, yaw, 0 });
    zox_setv(camera, LocalRotation3D, quaternion_rotate(quaternion_inverse(zox_getv(head, Rotation3D)), look));
}

byte zox_tst_sky_day_ready(ecs* world) {
    if (zox_tst_sky_day_index < 0) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_sky_day_ground = zox_getv(zox_tst_player_character(world), Position3D).y;
        zox_tst_sky_day_index = 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity realm = zox_get_link(world, character, RealmLink);
    float3 position = zox_getv(character, Position3D);
    position.y = zox_tst_sky_day_ground + 40;
    zox_setv(character, Position3D, position);
    zox_setv(character, Velocity3D, float3_zero);
    zox_setv(character, RenderDisabled, 1);
    const int i = zox_tst_sky_day_index;
    if (i >= zox_tst_sky_day_times) {
        return !zox_screenshot_request[0];
    }
    zox_tst_waiting = "the sky at each time of day";
    zox_tst_progress(i * 1000 + zox_tst_sky_day_frame);
    const float time_of_day = zox_tst_sky_day_of[i];
    zox_setv(realm, ClockScale, 0);
    zox_setv(realm, ClockTime, (time_of_day - sun_day_start + 1) * sun_day_length);
    const float3 sun = sun_direction_at(time_of_day);
    const float3 level = float3_normalize((float3) { sun.x, 0, sun.z });   // the horizon under it
    zox_tst_sky_day_look(world, character, i == 4 ? level : sun.y > 0 ? sun : float3_up);
    zox_tst_sky_day_sunset = realm_sunset_color(zox_getv(realm, Seed));
    const int f = ++zox_tst_sky_day_frame;
    if (f == 30) {
        char name[64];
        snprintf(name, sizeof(name), "sky_%s.ppm", zox_tst_sky_day_names[i]);
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(name));
    } else if (f > 30 && !zox_screenshot_request[0]) {
        zox_tst_sky_day_index++;
        zox_tst_sky_day_frame = 0;
    }
    return 0;
}

void zox_tst_sky_day_check(ecs* world) {
    (void) world;
    for (int i = 0; i < zox_tst_sky_day_times; i++) {
        char name[64];
        snprintf(name, sizeof(name), "sky_%s.ppm", zox_tst_sky_day_names[i]);
        const byte read = zox_tst_sky_day_read(zox_tst_path(name), &zox_tst_sky_day_middle[i], &zox_tst_sky_day_mean[i]);
        zox_tst_check(read, "no screenshot at [%s]", zox_tst_sky_day_names[i]);
        const float3 m = zox_tst_sky_day_middle[i];
        zox_log("[tst] sky_day: [%s] the middle [%.0f %.0f %.0f], the sky [%.0f]", zox_tst_sky_day_names[i], m.x, m.y, m.z, zox_tst_sky_day_mean[i]);
    }
    const float3* m = zox_tst_sky_day_middle;
    for (int i = 0; i < 3; i++) {
        const float brightest = float_max(m[i].x, float_max(m[i].y, m[i].z));
        zox_tst_check(brightest > 200, "the sun isn't in the middle at [%s] [%.0f %.0f %.0f]",
            zox_tst_sky_day_names[i], m[i].x, m[i].y, m[i].z);
    }
    // the sunset's colour: its strongest and weakest channels the sun's at dawn and dusk
    const float3 c = zox_tst_sky_day_sunset;
    zox_log("[tst] sky_day: the realm's sunset [%.2f %.2f %.2f]", c.x, c.y, c.z);
    const int strong = c.x >= c.y && c.x >= c.z ? 0 : c.y >= c.z ? 1 : 2;
    const int weak = c.x <= c.y && c.x <= c.z ? 0 : c.y <= c.z ? 1 : 2;
    for (int i = 0; i < 3; i += 2) {
        const float v[3] = { m[i].x, m[i].y, m[i].z };
        zox_tst_check(v[strong] >= v[0] && v[strong] >= v[1] && v[strong] >= v[2] && v[weak] <= v[0] && v[weak] <= v[1] && v[weak] <= v[2],
            "[%s]'s sun [%.0f %.0f %.0f] isn't the sunset's colour [%.2f %.2f %.2f]", zox_tst_sky_day_names[i], v[0], v[1], v[2], c.x, c.y, c.z);
    }
    zox_tst_check(zox_tst_sky_day_mean[3] < zox_tst_sky_day_mean[1] * 0.4f, "midnight's sky [%.0f] isn't dark beside noon's [%.0f]",
        zox_tst_sky_day_mean[3], zox_tst_sky_day_mean[1]);
}

void zox_tsts_sky_day(ecs* world) {
    zox_tst_add("sky_day", zox_tst_sky_day_ready, zox_tst_sky_day_check, 120);
}
