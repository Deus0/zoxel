// tst: body_views, a whole character (body, clothes, hat) in the void from 4 viewports, to look at
//  - bash bsh/body.sh (your settings, i.e. Block Res 5), then look at tst/body_views.png
//      (top left +z, top right +x, bottom left -z, bottom right above at an angle)
//  - on the main menu: its own realm (seed zox_tst_seed, else 666), a character of the player's kind (it wears
//      the realm's gear) spawned in the void far from the menu's scene, held still (no ground), 4 viewport cameras
//      round it on gray (spawn_viewport_camera, camera_look_at), the hud and menu view hidden
//  - checks: each view drew the character (its middle isn't all gray), each piece of clothes centred over its part
//  - logs the body's voxels from the front and side, and where its parts and clothes sit (as tst clothes)
//  - here, not in models: it needs the character, parts, equipment and player prefabs, models load before them
//  - model_views is one part on its own (src/models/tst/model_views.c), this is them put together
//  - later: animations, a strip of screenshots a few frames apart (todo.md)
extern char zox_screenshot_request[256];
extern byte nohud;
#define zox_tst_body_views_count 4
const double zox_tst_body_views_settle = 4.0;   // seconds after spawning, for the body, gear and its mesh
const float3 zox_tst_body_views_spot = { 0, 1000, 0 }; // the void, past the menu's sky
const char* zox_tst_body_views_names[zox_tst_body_views_count] = { "+z", "+x", "-z", "above" };
byte zox_tst_body_views_stage = 0;
int zox_tst_body_views_frames = 0;
double zox_tst_body_views_time = 0;
entity zox_tst_body_views_realm = 0;
entity zox_tst_body_views_character = 0;
entity zox_tst_body_views_cameras[zox_tst_body_views_count];

byte zox_tst_body_views_ready(ecs* world) {
    zox_tst_body_views_frames++;
    zox_tst_progress(zox_tst_body_views_stage * 10000 + zox_tst_body_views_frames);
    if (zox_tst_body_views_stage == 0) {
        zox_tst_waiting = "the game and app to spawn a realm into";
        if (!zox_valid(local_game) || !zox_valid(main_app) || zox_tst_body_views_frames < 5) {
            return 0;
        }
        const char* seed_text = getenv("zox_tst_seed");
        const lint seed = seed_text && seed_text[0] ? atoll(seed_text) : 666;
        zox_tst_body_views_realm = spawn_realm(world, local_game, seed);
        zox_setv(zox_tst_body_views_realm, GenerateRealm, zox_generate_realm_start);
        zox_tst_body_views_stage = 1;
        zox_tst_body_views_frames = 0;
        return 0;
    }
    const entity realm = zox_tst_body_views_realm;
    if (zox_tst_body_views_stage == 1) {
        zox_tst_waiting = "the realm to generate its body parts";
        if (zox_getv(realm, GenerateRealm) || !zox_tst_shapes_find(world, realm, "Chest") ||
            !zox_tst_shapes_find(world, realm, "Head")) {
            return 0;
        }
        // the gear finishes a little after
        if (zox_tst_body_views_frames < 60) {
            return 0;
        }
        zox_tst_body_views_character = spawn_character3(world, prefab_character3_player, realm, 0, 666, 0, 0,
            zox_tst_body_views_spot, quaternion_identity, "body_views");
        // on show, not playing: no terrain to be inside of, nothing to aim at, no crosshair (their systems want both)
        zox_remove(zox_tst_body_views_character, InsideBlock);
        zox_remove(zox_tst_body_views_character, RaycastRange);
        zox_remove(zox_tst_body_views_character, RaycastVoxelData);
        zox_tst_body_views_stage = 2;
        zox_tst_body_views_time = zox_current_time;
        zox_tst_body_views_frames = 0;
        return 0;
    }
    const entity character = zox_tst_body_views_character;
    if (!zox_valid(character)) {
        zox_tst_check(0, "the character is gone");
        return 1;
    }
    // held in the void, nothing to stand on
    zox_setv(character, Position3D, zox_tst_body_views_spot);
    zox_setv(character, Rotation3D, quaternion_identity);
    if (zox_has(character, Velocity3D)) {
        zox_setv(character, Velocity3D, float3_zero);
    }
    if (zox_tst_body_views_stage == 2) {
        zox_tst_waiting = "the body and gear to settle";
        if (zox_current_time - zox_tst_body_views_time < zox_tst_body_views_settle) {
            return 0;
        }
        // round its middle, far enough its bounds fit (Bounds3D: half its size)
        const float3 target = zox_tst_body_views_spot;
        const float3 bounds = zox_getv(character, Bounds3D);
        const float radius = float3_length(bounds);
        const float fov = 50;
        const float distance = radius / sinf(fov * 0.5f * degreesToRadians) * 1.1f;
        const float3 directions[zox_tst_body_views_count] = {
            { 0, 0.15f, 1 },
            { 1, 0.15f, 0 },
            { 0, 0.15f, -1 },
            { 0.6f, 0.9f, 0.6f } };
        for (int i = 0; i < zox_tst_body_views_count; i++) {
            const float3 position = float3_add(target, float3_scale(float3_normalize(directions[i]), distance));
            const entity camera = spawn_viewport_camera(world, main_app, position, camera_look_at(position, target), fov,
                viewport_grid_cell(i, 2, 2));
            zox_setv(camera, Color, ((color) { 96, 96, 96, 255 }));
            zox_tst_body_views_cameras[i] = camera;
        }
        nohud = 1;
        const entity game_view = zox_valid(main_cameras[0]) ? zox_get_link(world, main_cameras[0], RenderTextureLink) : 0;
        if (zox_valid(game_view)) {
            zox_setv(game_view, RenderDisabled, 1);
        }
        zox_log("[tst] body: half size [%.2f %.2f %.2f], views [%.2f] away", bounds.x, bounds.y, bounds.z, distance);
        zox_tst_body_views_stage = 3;
        zox_tst_body_views_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the body views' screenshot";
    if (zox_tst_body_views_stage == 3) {
        if (zox_tst_body_views_frames < 20) {
            return 0;
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("body_views.ppm"));
        zox_tst_body_views_stage = 4;
        return 0;
    }
    return !zox_screenshot_request[0];
}

void zox_tst_body_views_check(ecs* world) {
    // the body's voxels, front and side, with where its clothes sit (as tst clothes logs)
    const entity character = zox_tst_body_views_character;
    const entity chest_slot = zox_valid(character) ? zox_get_child_by_id(world, character, zox_id(Body)) : 0;
    if (zox_valid(chest_slot)) {
        zox_tst_cloth clothes[zox_tst_clothes_max];
        int count = 0;
        zox_tst_clothes_find(world, chest_slot, clothes, &count);
        for (int i = 0; i < count; i++) {
            if (zox_valid(clothes[i].item)) {
                const byte3 p = zox_getv(clothes[i].slot, PartPosition), sz = zox_getv(clothes[i].slot, PartSize);
                zox_log("[tst] cloth [%s] at [%ix%ix%i] size [%ix%ix%i]", zox_getn(clothes[i].item), p.x, p.y, p.z, sz.x, sz.y, sz.z);
                // centred over what it covers on x and z (the slot's parent holds the part)
                //  (a shirt sized before its model finished came out 64 deep, flush with the chest's back)
                const entity over = zox_get_parent(world, clothes[i].slot);
                if (zox_valid(over) && zox_has(over, PartPosition) && zox_valid(zox_get_link(world, over, DataLink))) {
                    const byte3 op = zox_getv(over, PartPosition), os = zox_getv(over, PartSize);
                    const int dx = abs((2 * p.x + sz.x) - (2 * op.x + os.x));
                    const int dz = abs((2 * p.z + sz.z) - (2 * op.z + os.z));
                    zox_tst_check(dx <= 2 && dz <= 2, "[%s] isn't centred over its part: off by [%.1f] x, [%.1f] z",
                        zox_getn(clothes[i].item), dx / 2.0f, dz / 2.0f);
                }
            }
        }
        zox_tst_clothes_log_parts(world, chest_slot);
        zox_tst_clothes_log_body(world, character, clothes, count);
    }
    int2 size;
    byte* pixels = zox_tst_load_ppm(zox_tst_path("body_views.ppm"), &size);
    zox_tst_check(pixels, "no screenshot [%s]", zox_tst_path("body_views.ppm"));
    if (!pixels) {
        return;
    }
    // the middle half of each view isn't all the gray background
    const byte gray[3] = { 96, 96, 96 };
    const int2 half = { size.x / 2, size.y / 2 };
    for (int i = 0; i < zox_tst_body_views_count; i++) {
        const int2 corner = { (i % 2) * half.x, (i / 2) * half.y };
        int drawn = 0, total = 0;
        for (int y = corner.y + half.y / 4; y < corner.y + half.y * 3 / 4; y++) {
            for (int x = corner.x + half.x / 4; x < corner.x + half.x * 3 / 4; x++) {
                drawn += zox_tst_pixel_difference(zox_tst_ppm_pixel(pixels, size, x, y), gray) > 24;
                total++;
            }
        }
        zox_log("[tst] view [%s]: [%.1f%%] of its middle is the character", zox_tst_body_views_names[i], 100.0f * drawn / total);
        zox_tst_check(drawn * 50 > total, "view [%s] didn't draw the character", zox_tst_body_views_names[i]);
    }
    free(pixels);
}
