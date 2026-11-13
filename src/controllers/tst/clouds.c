// tst: sky_clouds, clouds over the realm (src/sky): its children, high above, drifting with the wind
//  - the player lifted, its body hidden and looking up, a screenshot (tst/sky_clouds.ppm) to look at, it logs
//    how much of a band above the middle is cloud (near white)
//  - run: bash bsh/tst.sh sky_clouds, add --1080p to see it at the game's size (no hud: tst.sh adds --nohud)
extern char zox_screenshot_request[256];
byte zox_tst_clouds_stage = 0;
double zox_tst_clouds_time = 0;
entity zox_tst_cloud = 0;
float3 zox_tst_cloud_start;
float zox_tst_clouds_ground = 0;

// a realm's clouds, the first in first
int zox_tst_realm_clouds(ecs* world, entity realm, entity* first) {
    int count = 0;
    ecs_iter_t children = zox_children(world, realm);
    while (ecs_children_next(&children)) {
        for (int i = 0; i < children.count; i++) {
            if (zox_has(children.entities[i], Cloud)) {
                if (!count && first) {
                    *first = children.entities[i];
                }
                count++;
            }
        }
    }
    return count;
}

byte zox_tst_sky_clouds_ready(ecs* world) {
    if (!zox_tst_clouds_stage) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        const entity character = zox_get_link(world, dbg_player, CharacterLink);
        zox_tst_clouds_ground = zox_getv(character, Position3D).y;
        zox_tst_clouds_stage = 1;
        zox_tst_clouds_time = zox_current_time;
    }
    zox_tst_waiting = "clouds, then the sky screenshot";
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    float3 position = zox_getv(character, Position3D);
    position.y = zox_tst_clouds_ground + 40;
    zox_setv(character, Position3D, position);
    zox_setv(character, Velocity3D, float3_zero);
    // the body hidden, the camera sits in its head (looking up saw the inside of its hat)
    zox_setv(character, RenderDisabled, 1);
    // look up: the camera under the head bone is what mouse look pitches (controllers3/sys/rotation.c)
    const entity skeleton = get_skeleton(world, character);
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    const entity head_camera = zox_valid(head) ? zox_get_child_by_id(world, head, zox_id(Camera)) : 0;
    if (zox_valid(head_camera)) {
        zox_setv(head_camera, LocalRotation3D, euler_to_quaternion((float3) { 0.5f, is_camera_positive_z ? M_PI : 0, 0 }));
    }
    zox_tst_progress(zox_current_time - zox_tst_clouds_time);
    if (zox_tst_clouds_stage == 1) {
        const entity realm = zox_get_link(world, character, RealmLink);
        if (!zox_tst_cloud && !zox_tst_realm_clouds(world, realm, &zox_tst_cloud)) {
            return 0;
        }
        if (!zox_tst_cloud_start.y) {
            zox_tst_cloud_start = zox_getv(zox_tst_cloud, Position3D);
            zox_tst_clouds_time = zox_current_time;
        }
        if (zox_current_time - zox_tst_clouds_time < 3) {
            return 0;
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("sky_clouds.ppm"));
        zox_tst_clouds_stage = 2;
        return 0;
    }
    return !zox_screenshot_request[0];
}

// the share of grey (cloud) pixels in a band above the middle of the screenshot, -1 without one
float zox_tst_clouds_share(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        return -1;
    }
    int width = 0, height = 0, max = 0;
    const int read = fscanf(file, "P6 %d %d %d", &width, &height, &max);
    fgetc(file);
    if (read != 3 || width <= 0 || height <= 0) {
        fclose(file);
        return -1;
    }
    const int band_from = height * 15 / 100, band = height * 45 / 100;
    int cloud = 0, total = 0;
    byte rgb[3];
    fseek(file, (long) band_from * width * 3, SEEK_CUR);
    for (int y = band_from; y < band; y++) {
        for (int x = 0; x < width; x++) {
            if (fread(rgb, 1, 3, file) != 3) {
                break;
            }
            total++;
            // grey: clouds fade into the fog, the sky is a colour (red, purple, green on alien realms)
            const int high = rgb[0] > rgb[1] ? (rgb[0] > rgb[2] ? rgb[0] : rgb[2]) : (rgb[1] > rgb[2] ? rgb[1] : rgb[2]);
            const int low = rgb[0] < rgb[1] ? (rgb[0] < rgb[2] ? rgb[0] : rgb[2]) : (rgb[1] < rgb[2] ? rgb[1] : rgb[2]);
            cloud += high > 90 && (high - low) * 4 < high;
        }
    }
    fclose(file);
    return total ? 100.0f * cloud / total : 0;
}

void zox_tst_sky_clouds_check(ecs* world) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    zox_tst_check(zox_has(realm, CloudCentre), "the realm has no clouds (no CloudCentre)");
    const int count = zox_tst_realm_clouds(world, realm, NULL);
    zox_tst_check(count >= clouds_count, "the realm has [%i] clouds, under [%i]", count, clouds_count);
    zox_tst_check(zox_valid(zox_tst_cloud), "no cloud to follow");
    if (!zox_valid(zox_tst_cloud)) {
        return;
    }
    const float3 now = zox_getv(zox_tst_cloud, Position3D);
    zox_tst_check(now.y > zox_tst_clouds_ground + 20, "a cloud is at [%.1f], the ground [%.1f]", now.y, zox_tst_clouds_ground);
    const float moved = float3_distance(now, zox_tst_cloud_start);
    zox_tst_check(moved > 1, "a cloud drifted [%.2f] in 3 seconds", moved);
    const float share = zox_tst_clouds_share(zox_tst_path("sky_clouds.ppm"));
    zox_tst_check(share >= 0, "no screenshot saved");
    zox_tst_check(share > 0.2f, "no cloud in the sky looking up, [%.2f%%] of the band", share);
    zox_log("[tst] [metric] sky_clouds [%.1f%%] of the sky band is cloud, [%i] clouds, drifted [%.1f]", share, count, moved);
}
