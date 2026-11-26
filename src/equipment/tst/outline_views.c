// tst: outline_views, camera outlines (doc/plans/outlines.md) drawn by the menu's render camera, on then off
//  - on the main menu: its own realm, two characters (one partly behind the other: inner edges and depth gaps),
//    one far away (thinner lines) and the see through slime model (tst vox_see_through's, instanced, its shell writes
//    no depth: the mask finds it), in the void on a plain background (no skybox), the menu's camera moved to them
//  - saves tst/outline_views/on.ppm, then the setting off (Camera Outlines) and off.ppm, to look at
//  - checks: the Camera Outlines setting is on the video tab, unsupported (greyed) only where the backend can't
//    (vulkan, gles2); where it can:
//      - lines: pixels darker with outlines on, round the models
//      - the slime's ring: darker pixels round its edge (a see through shell)
//      - the background away from the models is the same on and off
//  - run: bash bsh/tst.sh outline_views
extern byte nohud;
extern char zox_screenshot_request[256];
#define zox_tst_ov_characters 3
const double zox_tst_ov_settle = 3.0;    // seconds after they are dressed (their meshes remade with the gear)
const float3 zox_tst_ov_spot = { 0, 1000, 0 };
const color zox_tst_ov_background = { 150, 170, 190, 255 };
byte zox_tst_ov_stage = 0;
int zox_tst_ov_frames = 0;
double zox_tst_ov_time = 0;
entity zox_tst_ov_realm = 0;
entity zox_tst_ov_characters_[zox_tst_ov_characters];
float3 zox_tst_ov_positions[zox_tst_ov_characters];
entity zox_tst_ov_slime = 0;
float3 zox_tst_ov_slime_position;
float zox_tst_ov_slime_radius = 0;
float3 zox_tst_ov_camera_position;
float4 zox_tst_ov_camera_rotation;
float4x4 zox_tst_ov_view_projection;

// no skybox (Disabled, out of every render query; the new camera's spawns a frame after it, so done twice)
void zox_tst_ov_no_skybox(ecs* world) {
    entity skyboxes[16];
    int count = 0;
    iter it = zox_each(world, RenderAtCamera);
    while (zox_each_next(it)) {
        for (int j = 0; j < it.count && count < 16; j++) {
            skyboxes[count++] = it.entities[j];
        }
    }
    for (int i = 0; i < count; i++) {
        if (!zox_has(skyboxes[i], Disabled)) {
            zox_add(skyboxes[i], Disabled);
        }
    }
}

// every character dressed: each wears something with a model (the gear comes a while after the body)
byte zox_tst_ov_dressed(ecs* world) {
    for (int i = 0; i < zox_tst_ov_characters; i++) {
        const entity chest = zox_get_child_by_id(world, zox_tst_ov_characters_[i], zox_id(Body));
        if (!zox_valid(chest)) {
            return 0;
        }
        zox_tst_cloth clothes[zox_tst_clothes_max];
        int count = 0;
        zox_tst_clothes_find(world, chest, clothes, &count);
        int worn = 0;
        for (int j = 0; j < count; j++) {
            worn += zox_valid(clothes[j].item) && zox_valid(clothes[j].model);
        }
        if (!worn) {
            return 0;
        }
    }
    return 1;
}

// holds the characters still in the void (nothing to stand on), the camera on them
void zox_tst_ov_hold(ecs* world) {
    for (int i = 0; i < zox_tst_ov_characters; i++) {
        const entity e = zox_tst_ov_characters_[i];
        if (!zox_valid(e)) {
            continue;
        }
        zox_setv(e, Position3D, zox_tst_ov_positions[i]);
        zox_setv(e, Rotation3D, euler_to_quaternion((float3) { 0, (i == 0 ? 0.5f : -0.4f), 0 }));
        if (zox_has(e, Velocity3D)) {
            zox_setv(e, Velocity3D, float3_zero);
        }
    }
    if (zox_tst_ov_stage >= 3) {
        zox_setv(main_cameras[0], Position3D, zox_tst_ov_camera_position);
        zox_setv(main_cameras[0], Rotation3D, zox_tst_ov_camera_rotation);
    }
}

byte zox_tst_outline_views_ready(ecs* world) {
    zox_tst_ov_frames++;
    zox_tst_progress(zox_tst_ov_stage * 10000 + zox_tst_ov_frames);
    if (zox_tst_ov_stage == 0) {
        zox_tst_waiting = "the game, app and menu camera";
        if (!zox_valid(local_game) || !zox_valid(main_app) || !zox_valid(main_cameras[0]) || zox_tst_ov_frames < 5) {
            return 0;
        }
        // unsupported: only the setting to check
        if (!zox_gpu_outlines_supported()) {
            return 1;
        }
        zox_tst_ov_realm = spawn_realm(world, local_game, 666);
        zox_setv(zox_tst_ov_realm, GenerateRealm, zox_generate_realm_start);
        zox_tst_vst_spawn(world);
        zox_tst_ov_stage = 1;
        zox_tst_ov_frames = 0;
        return 0;
    }
    const entity realm = zox_tst_ov_realm;
    if (zox_tst_ov_stage == 1) {
        zox_tst_waiting = "the realm's body parts and the slime's mesh";
        const entity slime_mesh = get_max_model_mesh(world, zox_tst_vst_model);
        if (zox_getv(realm, GenerateRealm) || !zox_tst_shapes_find(world, realm, "Chest") ||
            !zox_tst_shapes_find(world, realm, "Head") || !zox_valid(slime_mesh) || !mesh_index_count(world, slime_mesh) ||
            zox_tst_ov_frames < 60) {
            return 0;
        }
        for (int i = 0; i < zox_tst_ov_characters; i++) {
            zox_tst_ov_positions[i] = zox_tst_ov_spot;
            zox_tst_ov_characters_[i] = spawn_character3(world, prefab_character3_skeleton, realm, 0, 666 + i * 7, 0, 0,
                zox_tst_ov_spot, quaternion_identity, "outline_views");
            zox_add(zox_tst_ov_characters_[i], PlayerCharacter);
            zox_remove(zox_tst_ov_characters_[i], InsideBlock);
            zox_remove(zox_tst_ov_characters_[i], RaycastRange);
            zox_remove(zox_tst_ov_characters_[i], RaycastVoxelData);
        }
        zox_tst_ov_slime = zox_ins(world, prefab_renderer_instance);
        zox_set_link(world, zox_tst_ov_slime, InstanceLink, slime_mesh);
        zox_set_unique_name(zox_tst_ov_slime, "outline_views_slime");
        zox_tst_ov_stage = 2;
        zox_tst_ov_time = zox_current_time;
        zox_tst_ov_frames = 0;
        return 0;
    }
    for (int i = 0; i < zox_tst_ov_characters; i++) {
        if (!zox_valid(zox_tst_ov_characters_[i])) {
            zox_tst_check(0, "character [%i] is gone", i);
            return 1;
        }
    }
    zox_tst_ov_hold(world);
    if (zox_tst_ov_stage == 2) {
        zox_tst_waiting = "the bodies dressed, then settled";
        if (!zox_tst_ov_dressed(world)) {
            zox_tst_ov_time = zox_current_time;
            return 0;
        }
        // their meshes remade with the gear
        if (zox_current_time - zox_tst_ov_time < zox_tst_ov_settle) {
            return 0;
        }
        // laid out by the body's size: the front one, one behind it to the left, one far behind to the right,
        //  the slime in front to the right on the front one's feet
        const float h = zox_getv(zox_tst_ov_characters_[0], Bounds3D).y;
        const float3 offsets[zox_tst_ov_characters] = { { 0, 0, 0 }, { -0.9f * h, 0, -1.6f * h }, { 1.6f * h, 0, -9 * h } };
        for (int i = 0; i < zox_tst_ov_characters; i++) {
            zox_tst_ov_positions[i] = float3_add(zox_tst_ov_spot, offsets[i]);
        }
        const float voxel = zox_getv(zox_tst_vst_model, BlockScale);
        const float model_size = octree_size(zox_getv(zox_tst_vst_model, NodeDepth)) * voxel;
        zox_tst_ov_slime_radius = 0.5f * h;
        zox_tst_ov_slime_position = float3_add(zox_tst_ov_spot, (float3) { 1.0f * h, -h + zox_tst_ov_slime_radius, 0.4f * h });
        zox_setv(zox_tst_ov_slime, Position3D, zox_tst_ov_slime_position);
        zox_setv(zox_tst_ov_slime, Rotation3D, quaternion_identity);
        zox_setv(zox_tst_ov_slime, Scale1, 2 * zox_tst_ov_slime_radius / model_size);
        // the menu's camera in front, a little above; a plain clear, no skybox, vignette, blur or hud
        const float3 target = float3_add(zox_tst_ov_spot, (float3) { 0.25f * h, -0.1f * h, -0.4f * h });
        zox_tst_ov_camera_position = float3_add(zox_tst_ov_spot, (float3) { 0.3f * h, 0.35f * h, 4.2f * h });
        zox_tst_ov_camera_rotation = camera_look_at(zox_tst_ov_camera_position, target);
        const entity camera = main_cameras[0];
        zox_setv(camera, Color, zox_tst_ov_background);
        zox_setv(camera, FieldOfView, 50);      // as the prototype's views (doc/plans/outlines.md)
        zox_setv(camera, CameraVignette, 0);
        zox_setv(camera, CameraBlur, 0);
        zox_tst_ov_no_skybox(world);
        nohud = 1;
        zox_tst_ov_stage = 3;
        zox_tst_ov_frames = 0;
        zox_tst_ov_hold(world);
        return 0;
    }
    if (zox_screenshot_request[0]) {
        return 0;
    }
    if (zox_tst_ov_stage == 3) {
        zox_tst_waiting = "the view with outlines";
        if (zox_tst_ov_frames == 5) {
            zox_tst_ov_no_skybox(world);
        }
        if (zox_tst_ov_frames < 20) {
            return 0;
        }
        zox_tst_ov_view_projection = zox_getv(main_cameras[0], ViewProjectionMatrix);
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("on.ppm"));
        zox_tst_ov_stage = 4;
        zox_tst_ov_frames = 0;
        return 0;
    }
    if (zox_tst_ov_stage == 4) {
        zox_tst_waiting = "the view without outlines";
        if (zox_tst_ov_frames == 1) {
            set_app_setting_byte(world, main_app, "Camera Outlines", 0);
        }
        if (zox_tst_ov_frames < 10) {
            return 0;
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("off.ppm"));
        zox_tst_ov_stage = 5;
        return 0;
    }
    return 1;
}

// where a point lands in the screenshot (top row first), pixels
static inline float2 zox_tst_ov_screen(float3 point, int2 size) {
    const float4 clip = float4x4_multiply_float4(zox_tst_ov_view_projection, (float4) { point.x, point.y, point.z, 1 });
    return (float2) { (0.5f + 0.5f * clip.x / clip.w) * size.x, (0.5f - 0.5f * clip.y / clip.w) * size.y };
}

void zox_tst_outline_views_check(ecs* world) {
    // the setting: on the video tab, unsupported only where the backend can't
    entity setting = 0;
    iter it = zox_children(world, main_app);
    while (zox_children_next(it)) {
        for (int j = 0; j < it.count; j++) {
            if (zox_has(it.entities[j], SettingCameraOutlines)) {
                setting = it.entities[j];
            }
        }
    }
    zox_tst_check(setting && zox_has(setting, SettingTabVideo), "no Camera Outlines setting on the video tab");
    zox_tst_check(setting && zox_has(setting, SettingUnsupported) == !zox_gpu_outlines_supported(),
        "Camera Outlines is [%s] unsupported, the backend [%s] it", setting && zox_has(setting, SettingUnsupported) ? "" :
        "not", zox_gpu_outlines_supported() ? "supports" : "doesn't support");
    if (!zox_gpu_outlines_supported()) {
        zox_log("[tst] outline_views: unsupported here, the setting greyed");
        return;
    }
    zox_tst_check(!zox_camera_outlines, "the setting didn't turn outlines off");
    int2 size, size_off;
    byte* on = zox_tst_load_ppm(zox_tst_path("on.ppm"), &size);
    byte* off = zox_tst_load_ppm(zox_tst_path("off.ppm"), &size_off);
    zox_tst_check(on && off && int2_equals(size, size_off), "the screenshots weren't saved");
    if (!on || !off || !int2_equals(size, size_off)) {
        return;
    }
    // the slime's middle and radius on screen: its ring is just past its edge
    const float2 slime = zox_tst_ov_screen(zox_tst_ov_slime_position, size);
    const float2 slime_edge = zox_tst_ov_screen(float3_add(zox_tst_ov_slime_position,
        (float3) { zox_tst_ov_slime_radius, 0, 0 }), size);
    const float slime_radius = fabsf(slime_edge.x - slime.x);
    int darker = 0, slime_ring = 0, background = 0, background_changed = 0;
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            const byte* a = zox_tst_ppm_pixel(on, size, x, y);
            const byte* b = zox_tst_ppm_pixel(off, size, x, y);
            const int dark = (b[0] + b[1] + b[2]) - (a[0] + a[1] + a[2]);
            darker += dark > 60;
            const float dx = x - slime.x, dy = y - slime.y;
            const float distance = sqrtf(dx * dx + dy * dy);
            slime_ring += dark > 60 && distance > slime_radius * 0.7f && distance < slime_radius * 1.4f;
            // the top band, over the models' heads, and the bottom corners: background only
            if (y < size.y / 10 || (y > size.y * 9 / 10 && (x < size.x / 10 || x > size.x * 9 / 10))) {
                background++;
                background_changed += zox_tst_pixel_difference(a, b) > 12;
            }
        }
    }
    zox_log("[tst] outline_views: [%ix%i] darker with outlines [%i], round the slime [%i] (at [%.0f %.0f] r [%.0f]), "
        "background changed [%i/%i]", size.x, size.y, darker, slime_ring, slime.x, slime.y, slime_radius,
        background_changed, background);
    zox_tst_check(darker > size.x * size.y / 200, "few pixels outlined [%i]", darker);
    zox_tst_check(slime_ring > slime_radius * 2, "the slime has no ring [%i] (its see through shell)", slime_ring);
    zox_tst_check(background_changed * 100 < background, "the background changed [%i/%i]", background_changed, background);
    free(on);
    free(off);
}

// a bigger window than tests' default, lines are a few pixels: easier to look at
void zox_tst_outline_views_setup() {
    window_size_override = (int2) { 1200, 900 };
}

void zox_tsts_outline_views(ecs* world) {
    zox_tst_add("outline_views", zox_tst_outline_views_ready, zox_tst_outline_views_check, 120);
    zox_tst_set_setup("outline_views", zox_tst_outline_views_setup, 1);
}
