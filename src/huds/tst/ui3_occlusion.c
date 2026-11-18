// tst: ui3_occlusion, a 3D ui behind the world is hidden by it, at a distance too (a depth offset of 5000 a layer
//  pulled far 3D uis through buildings, element3_render_system now nudges layers toward the camera in the world)
//  - on flat land (zox_flatlands), holds the player's character up above the ground, its head camera looking a little
//      down (far off it meets the
//      ground)
//  - a red 3D frame (layer 2, as text on a frame) on the camera's view: past where it meets the ground (under it),
//      the screen has no red; far off in front of the ground its layers (a blue frame on it) don't fight; near the
//      camera, in front of the ground, it shows
//  - run: bash bsh/tst.sh ui3_occlusion
#define zox_tst_u3_height 24
int zox_tst_u3_stage = 0;
int zox_tst_u3_frames = 0;
entity zox_tst_u3_canvas = 0;
float3 zox_tst_u3_at;
int zox_tst_u3_hidden_red = -1, zox_tst_u3_shown_red = -1, zox_tst_u3_hidden_distance = 0;
int zox_tst_u3_far_red = -1, zox_tst_u3_far_blue = -1;
int zox_tst_u3_ground = -1;     // the flat land's height (blocks)
int zox_tst_u3_near_blue = -1;

// pixels of a pure colour (red 0, green 1, blue 2) in a screenshot (the frames; the land's are darker), -1 if none
static inline int zox_tst_u3_pixels(const char* path, int channel) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        return -1;
    }
    int width = 0, height = 0, max = 0, red = 0;
    if (fscanf(file, "P6 %d %d %d", &width, &height, &max) != 3 || width <= 0 || height <= 0) {
        fclose(file);
        return -1;
    }
    fgetc(file);
    unsigned char rgb[3];
    for (int i = 0; i < width * height && fread(rgb, 1, 3, file) == 3; i++) {
        red += rgb[channel] > 220 && rgb[(channel + 1) % 3] < 40 && rgb[(channel + 2) % 3] < 40;
    }
    fclose(file);
    return red;
}

// the ground's height (blocks) at a block column, from its tunk's height map, -1 if it isn't there
static inline int zox_tst_u3_ground_at(ecs* world, entity terrain, int2 block) {
    const int length = octree_size(zox_getv(terrain, NodeDepth));
    const int2 tunk_at = { (int) floorf((float) block.x / length), (int) floorf((float) block.y / length) };
    const entity tunk = int2_hashmap_get(zox_get(terrain, TunkLinks)->value, tunk_at);
    if (!zox_valid(tunk) || !zox_has(tunk, HeightMap)) {
        return -1;
    }
    const HeightMap* heights = zox_get(tunk, HeightMap);
    if (heights->length != length * length) {
        return -1;
    }
    const int2 local = { block.x - tunk_at.x * length, block.y - tunk_at.y * length };
    return heights->value[int2_array_index(local, int2_single(length))];
}

// the head camera, looking down
static inline entity zox_tst_u3_camera(ecs* world, entity character) {
    const entity skeleton = get_skeleton(world, character);
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    return zox_valid(head) ? zox_get_child_by_id(world, head, zox_id(Camera)) : 0;
}

// a point along the camera's view (the way that looks down)
static inline float3 zox_tst_u3_along(ecs* world, entity camera, float distance) {
    float3 view = float4_rotate_float3(zox_getv(camera, Rotation3D), (float3) { 0, 0, -1 });
    if (view.y > 0) {
        view = float3_scale(view, -1);
    }
    return float3_add(zox_getv(camera, Position3D), float3_scale(view, distance));
}

// a canvas with a frame of a colour on it, its layer
static inline entity zox_tst_u3_frame_canvas(ecs* world, float3 at, color c, byte layer) {
    const entity canvas = spawn_canvas3(world, prefab_canvas3, at, 2, int2_single(64));
    const entity frame = spawn_element3(world, prefab_frame3, canvas, float2_centre, int2_zero, int2_single(64));
    zox_setv(frame, FillColor, c);
    zox_setv(frame, OutlineColor, c);
    zox_setv(frame, Layer, layer);
    return canvas;
}

static inline void zox_tst_u3_spawn(ecs* world, float3 at) {
    zox_tst_u3_canvas = zox_tst_u3_frame_canvas(world, at, (color) { 255, 0, 0, 255 }, 2);
    // a blue frame on it, a layer up (as text on a frame)
    const entity inner = spawn_element3(world, prefab_frame3, zox_tst_u3_canvas, float2_centre, int2_zero, int2_single(32));
    zox_setv(inner, FillColor, ((color) { 0, 0, 255, 255 }));
    zox_setv(inner, OutlineColor, ((color) { 0, 0, 255, 255 }));
    zox_setv(inner, Layer, 3);
}

// flat bare land: the ground the same height everywhere, its view meets it where expected
void zox_tst_ui3_occlusion_setup() {
    zox_flatlands = 1;
    zox_disable_towns = 1;          // nothing standing in front of the far frame
    zox_disable_vegetation = 1;
}

byte zox_tst_ui3_occlusion_ready(ecs* world) {
    if (!zox_tst_u3_stage && !zox_tst_u3_frames && !zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const float scale = zox_getv(terrain, BlockScale);
    const entity camera = zox_tst_u3_camera(world, character);
    zox_tst_u3_frames++;
    zox_tst_progress(zox_tst_u3_stage);
    if (zox_tst_u3_stage == 0) {
        const float3 p = zox_getv(character, Position3D);
        const int ground = zox_tst_u3_ground_at(world, terrain, (int2) { (int) floorf(p.x / scale), (int) floorf(p.z / scale) });
        zox_tst_u3_ground = ground;
        if (ground < 0 || !zox_valid(camera)) {
            zox_tst_check(0, "no ground height under the player, or no head camera");
            return 1;
        }
        zox_tst_u3_at = (float3) { p.x, (ground + zox_tst_u3_height) * scale, p.z };
        zox_tst_u3_stage = 1;
        zox_tst_u3_frames = 0;
    }
    fog_density = 0;    // far off, fog tinted it out of red
    zox_setv(character, Position3D, zox_tst_u3_at);
    zox_setv(character, Velocity3D, float3_zero);
    zox_setv(character, RenderDisabled, 1);
    zox_setv(camera, LocalRotation3D, euler_to_quaternion((float3) { -0.5f, is_camera_positive_z ? M_PI : 0, 0 }));
    if (zox_tst_u3_stage == 1) {
        zox_tst_waiting = "the camera to settle";
        if (zox_tst_u3_frames < 40) {
            return 0;
        }
        // its view: which way along it is down
        const float3 eye = zox_getv(camera, Position3D);
        float3 view = float4_rotate_float3(zox_getv(camera, Rotation3D), (float3) { 0, 0, -1 });
        if (view.y > 0) {
            view = float3_scale(view, -1);
        }
        // along it until under the ground, then 3 blocks on (shallow: just under it, far off, as behind a wall)
        int distance = 1;
        for (; distance < 120; distance++) {
            const float3 at = float3_add(eye, float3_scale(view, distance * scale));
            if (at.y / scale < zox_tst_u3_ground) {
                break;
            }
        }
        if (distance >= 120) {
            zox_tst_check(0, "the camera's view never met the ground (its height maps)");
            return 1;
        }
        zox_tst_u3_hidden_distance = distance + 3;
        zox_tst_u3_spawn(world, float3_add(eye, float3_scale(view, zox_tst_u3_hidden_distance * scale)));
        zox_tst_u3_stage = 2;
        zox_tst_u3_frames = 0;
        return 0;
    }
    if (zox_tst_u3_stage == 2) {
        zox_tst_waiting = "the 3D ui under the ground";
        if (zox_tst_u3_frames == 30) {
            snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("ui3_hidden.ppm"));
        }
        if (zox_tst_u3_frames < 40) {
            return 0;
        }
        zox_tst_u3_hidden_red = zox_tst_u3_pixels(zox_tst_path("ui3_hidden.ppm"), 0);
        // far off but in front of the ground: its layers apart (blue on red)
        //  (lifted 10 blocks, over the trees and roofs that stood in front of it)
        const float3 distant = zox_tst_u3_along(world, camera, (zox_tst_u3_hidden_distance - 20) * scale);
        zox_setv(zox_tst_u3_canvas, Position3D, ((float3) { distant.x, distant.y + 10 * scale, distant.z }));
        zox_tst_u3_stage = 3;
        zox_tst_u3_frames = 0;
        return 0;
    }
    if (zox_tst_u3_stage == 3) {
        zox_tst_waiting = "the 3D ui far off";
        if (zox_tst_u3_frames == 30) {
            snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("ui3_far.ppm"));
        }
        if (zox_tst_u3_frames < 40) {
            return 0;
        }
        zox_tst_u3_far_red = zox_tst_u3_pixels(zox_tst_path("ui3_far.ppm"), 0);
        zox_tst_u3_far_blue = zox_tst_u3_pixels(zox_tst_path("ui3_far.ppm"), 2);
        // near the camera, in front of the ground
        zox_setv(zox_tst_u3_canvas, Position3D, zox_tst_u3_along(world, camera, 6 * scale));
        zox_tst_u3_stage = 4;
        zox_tst_u3_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the 3D ui in front";
    if (zox_tst_u3_frames == 30) {
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("ui3_shown.ppm"));
    }
    if (zox_tst_u3_frames < 40) {
        return 0;
    }
    zox_tst_u3_shown_red = zox_tst_u3_pixels(zox_tst_path("ui3_shown.ppm"), 0);
    zox_tst_u3_near_blue = zox_tst_u3_pixels(zox_tst_path("ui3_shown.ppm"), 2);
    zox_delete(zox_tst_u3_canvas);
    zox_setv(character, RenderDisabled, 0);
    return 1;
}

void zox_tst_ui3_occlusion_check(ecs* world) {
    (void) world;
    zox_log("[tst] [metric] ui3_occlusion red pixels: under the ground [%i] blocks away [%i], in front [%i]",
        zox_tst_u3_hidden_distance, zox_tst_u3_hidden_red, zox_tst_u3_shown_red);
    zox_tst_check(zox_tst_u3_shown_red > 200, "the 3D ui in front isn't seen (the test can't see it)");
    zox_tst_check(zox_tst_u3_hidden_red >= 0 && zox_tst_u3_hidden_red < 20, "the 3D ui under the ground shows through it");
    zox_log("[tst] [metric] ui3_occlusion far off [%i] blocks: red [%i] blue [%i], near blue [%i]", zox_tst_u3_hidden_distance - 20,
        zox_tst_u3_far_red, zox_tst_u3_far_blue, zox_tst_u3_near_blue);
    zox_tst_check(zox_tst_u3_near_blue > 0, "near, its layers fight: no blue on its red");
    // its inner frame (a quarter of it) over its outer, not fighting it
    zox_tst_check(zox_tst_u3_far_blue > 0 && zox_tst_u3_far_blue * 6 > zox_tst_u3_far_red, "far off, its layers fight: blue [%i] red [%i]",
        zox_tst_u3_far_blue, zox_tst_u3_far_red);
}

void zox_tsts_ui3_occlusion(ecs* world) {
    zox_tst_add("ui3_occlusion", zox_tst_ui3_occlusion_ready, zox_tst_ui3_occlusion_check, 90);
    zox_tst_set_setup("ui3_occlusion", zox_tst_ui3_occlusion_setup, 0);
}
