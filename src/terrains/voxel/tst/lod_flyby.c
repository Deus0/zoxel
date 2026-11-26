// tst: lod_flyby, a camera flying over the terrain as a player would, recorded to look at lods changing (flicker, pops)
//  - a world of its own, no player or menu (zox_tst_world_setup), a camera of its own its streamer (as a player's: lods
//    round it, chunks culled to its view), a few blocks over the land; once the land's loaded it flies a path (along z, then x) at a
//    speed in chunks a second (zox_tst_flyby_speed, 2 by default), looking ahead and a little down
//  - tst/lod_flyby/flyby.gif: the flight, each frame held its real time (plays at the game's speed)
//  - each frame against the two before it, as drawn (the gif's own frames):
//      - blinks: a pixel that changes for one frame and comes back (A, B, A): flying moves the picture steadily, a
//        frame of a hole, a dark mesh or a swap and back is what does this
//      - light pops: a pixel brighter or darker by much and the same colour (its hue kept): a face relit
//  - tst/lod_flyby/frame_0.png and frame_40.png: what it looks at; blink_<n>.png: the worst frames, the one before, the blink (magenta where it blinked), the one after
//  - logs both a frame (average, most), and the frames that stand out (over 4x the average)
//  - bash bsh/tools/flyby.sh (any tst.sh arguments: --vulkan, --gpu, --1080p, --cpu_meshing), medium terrain
#define zox_tst_flyby_threshold 40      // a channel's change that counts, of 255
#define zox_tst_flyby_worst 4           // blink frames saved
#define zox_tst_flyby_frames_max 4096
byte zox_tst_flyby_stage = 0;
entity zox_tst_flyby_streamer = 0;
entity zox_tst_flyby_camera = 0;
double zox_tst_flyby_time = 0;
int zox_tst_flyby_settled = 0;
float zox_tst_flyby_travelled = 0;      // chunks along the path
float zox_tst_flyby_speed = 2;
static const float3 zox_tst_flyby_path[] = { { 0, 0, 6 }, { 4, 0, 0 }, { 0, 0, -2 } };
#define zox_tst_flyby_legs ((int) (sizeof(zox_tst_flyby_path) / sizeof(zox_tst_flyby_path[0])))
// the last three frames taken (rgb, top row first), and when the newest was
byte* zox_tst_flyby_rgb[3] = { NULL, NULL, NULL };
int2 zox_tst_flyby_size = { 0, 0 };
double zox_tst_flyby_taken = -1;
int zox_tst_flyby_frames = 0;
int zox_tst_flyby_blinks[zox_tst_flyby_frames_max];
int zox_tst_flyby_pops[zox_tst_flyby_frames_max];
typedef struct {
    int frame;
    int blinks;
    byte* image;    // before, blink, after side by side, half size
} zox_tst_flyby_saved;
zox_tst_flyby_saved zox_tst_flyby_saves[zox_tst_flyby_worst];

// where the path is after travelled chunks, and which way it's heading
static inline float3 zox_tst_flyby_at(float travelled, float3* heading) {
    float3 at = float3_zero;
    *heading = (float3) { 1, 0, 0 };
    for (int i = 0; i < zox_tst_flyby_legs; i++) {
        const float3 leg = zox_tst_flyby_path[i];
        const float length = fabsf(leg.x) + fabsf(leg.y) + fabsf(leg.z);
        *heading = float3_scale(leg, 1.0f / length);
        if (travelled <= length) {
            return float3_add(at, float3_scale(*heading, travelled));
        }
        at = float3_add(at, leg);
        travelled -= length;
    }
    return at;
}

static inline float zox_tst_flyby_length() {
    float length = 0;
    for (int i = 0; i < zox_tst_flyby_legs; i++) {
        const float3 leg = zox_tst_flyby_path[i];
        length += fabsf(leg.x) + fabsf(leg.y) + fabsf(leg.z);
    }
    return length;
}

// the land's height at a world x z (its top solid voxel down the chunks there), or fallback if none's loaded
static inline float zox_tst_flyby_ground(ecs* world, entity terrain, float x, float z, float fallback) {
    const float scale = zox_getv(terrain, BlockScale);
    const int length = octree_size(zox_getv(terrain, NodeDepth));
    const ChunkLinks* links = zox_get(terrain, ChunkLinks);
    const int bx = (int) floorf(x / scale), bz = (int) floorf(z / scale);
    const int cx = (int) floorf(bx / (float) length), cz = (int) floorf(bz / (float) length);
    const byte3 local = { bx - cx * length, 0, bz - cz * length };
    for (int cy = 8; cy >= -8; cy--) {
        const entity chunk = int3_hashmap_get(links->value, (int3) { cx, cy, cz });
        if (!zox_valid(chunk) || !zox_has(chunk, VoxelNode)) {
            continue;
        }
        const VoxelNode* voxels = zox_get(chunk, VoxelNode);
        const byte depth = zox_getv(chunk, NodeDepth);
        const int size = octree_size(depth);
        const int step = length / size;
        for (int y = size - 1; y >= 0; y--) {
            if (getv_VoxelNode(voxels, depth, (byte3) { local.x / step, y, local.z / step })) {
                return ((cy * length) + (y + 1) * step) * scale;
            }
        }
    }
    return fallback;
}

float zox_tst_flyby_height = 0;     // the camera's height, eased over the land

// the camera (it streams) at a point of the path: over the land as a player flying, looking ahead and a little down
static inline void zox_tst_flyby_place(ecs* world, entity terrain, float chunk, float travelled, byte snap) {
    float3 heading;
    const float3 at = zox_tst_flyby_at(travelled, &heading);
    const float scale = zox_getv(terrain, BlockScale);
    const float x = (at.x + 0.5f) * chunk, z = (at.z + 0.5f) * chunk;
    // over the higher of the land here and a little ahead, a few blocks up, eased (no jolts over each step)
    const float ground = fmaxf(zox_tst_flyby_ground(world, terrain, x, z, chunk),
        zox_tst_flyby_ground(world, terrain, x + heading.x * chunk, z + heading.z * chunk, chunk));
    const float wanted = ground + scale * 10;
    zox_tst_flyby_height = snap ? wanted : zox_tst_flyby_height + (wanted - zox_tst_flyby_height) * fminf(1, 2 * zox_delta_time);
    const float3 eye = { x, zox_tst_flyby_height, z };
    // ~15 degrees down
    const float3 target = float3_add(eye, (float3) { heading.x * chunk, -chunk * 0.27f, heading.z * chunk });
    // its local transform too: the transforms build its Position3D and Rotation3D from them each frame (set alone,
    //  it stayed at the origin, under the land)
    const float4 rotation = camera_look_at(eye, target);
    zox_setv(zox_tst_flyby_camera, Position3D, eye);
    zox_setv(zox_tst_flyby_camera, Rotation3D, rotation);
    zox_setv(zox_tst_flyby_camera, LocalPosition3D, eye);
    zox_setv(zox_tst_flyby_camera, LocalRotation3D, rotation);
}

static inline int zox_tst_flyby_light(const byte* p) {
    return (p[0] * 3 + p[1] * 6 + p[2]) / 10;
}

// keeps a blink frame if it's among the worst: the frame before, it (blinks magenta), after, side by side at half size
static void zox_tst_flyby_keep(int blinks, const byte* mask) {
    int slot = -1;
    for (int i = 0; i < zox_tst_flyby_worst; i++) {
        if (!zox_tst_flyby_saves[i].image) {
            slot = i;
            break;
        }
        if (slot < 0 || zox_tst_flyby_saves[i].blinks < zox_tst_flyby_saves[slot].blinks) {
            slot = i;
        }
    }
    if (zox_tst_flyby_saves[slot].image && zox_tst_flyby_saves[slot].blinks >= blinks) {
        return;
    }
    const int2 size = zox_tst_flyby_size;
    const int w = size.x / 2, h = size.y / 2;
    byte* image = zox_tst_flyby_saves[slot].image;
    if (!image) {
        image = malloc((size_t) w * 3 * h * 3);
    }
    // the newest frame is [0]: the blink [1], before it [2]
    const byte* frames[3] = { zox_tst_flyby_rgb[2], zox_tst_flyby_rgb[1], zox_tst_flyby_rgb[0] };
    for (int f = 0; f < 3; f++) {
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                const size_t from = ((size_t) (y * 2) * size.x + x * 2);
                byte* to = image + ((size_t) y * w * 3 + f * w + x) * 3;
                const byte* rgb = frames[f] + from * 3;
                if (f == 1 && (mask[from] || mask[from + 1] || mask[from + size.x] || mask[from + size.x + 1])) {
                    to[0] = 255; to[1] = 0; to[2] = 255;
                } else {
                    to[0] = rgb[0]; to[1] = rgb[1]; to[2] = rgb[2];
                }
            }
        }
    }
    zox_tst_flyby_saves[slot] = (zox_tst_flyby_saved) { zox_tst_flyby_frames, blinks, image };
}

// a new frame taken by the gif: its blinks and light pops against the two before
static void zox_tst_flyby_frame() {
    const zox_gif* gif = &zox_gif_recorder;
    if (!gif->held || gif->last == zox_tst_flyby_taken) {
        return;
    }
    zox_tst_flyby_taken = gif->last;
    const int2 size = gif->size;
    const size_t pixels = (size_t) size.x * size.y;
    if (size.x != zox_tst_flyby_size.x || size.y != zox_tst_flyby_size.y) {
        for (int i = 0; i < 3; i++) {
            free(zox_tst_flyby_rgb[i]);
            zox_tst_flyby_rgb[i] = NULL;
        }
        zox_tst_flyby_size = size;
    }
    // the oldest's buffer takes the newest
    byte* newest = zox_tst_flyby_rgb[2] ? zox_tst_flyby_rgb[2] : malloc(pixels * 3);
    zox_tst_flyby_rgb[2] = zox_tst_flyby_rgb[1];
    zox_tst_flyby_rgb[1] = zox_tst_flyby_rgb[0];
    zox_tst_flyby_rgb[0] = newest;
    memcpy(newest, gif->held, pixels * 3);
    if (!zox_tst_flyby_rgb[2] || zox_tst_flyby_frames >= zox_tst_flyby_frames_max) {
        return;
    }
    const byte* a = zox_tst_flyby_rgb[2];
    const byte* b = zox_tst_flyby_rgb[1];
    const byte* c = zox_tst_flyby_rgb[0];
    byte* mask = calloc(pixels, 1);
    int blinks = 0, pops = 0;
    const int t = zox_tst_flyby_threshold;
    for (size_t i = 0; i < pixels; i++) {
        const byte* pa = a + i * 3;
        const byte* pb = b + i * 3;
        const byte* pc = c + i * 3;
        int ab = 0, bc = 0, ac = 0;
        for (byte k = 0; k < 3; k++) {
            ab = int_max(ab, abs(pa[k] - pb[k]));
            bc = int_max(bc, abs(pb[k] - pc[k]));
            ac = int_max(ac, abs(pa[k] - pc[k]));
        }
        if (ab > t && bc > t && ac < t / 2) {
            mask[i] = 1;
            blinks++;
        }
        // brighter or darker, its colour kept: each channel scaled about the same
        const int la = zox_tst_flyby_light(pb), lb = zox_tst_flyby_light(pc);
        if (abs(la - lb) > t && la > 8 && lb > 8) {
            byte same = 1;
            for (byte k = 0; k < 3 && same; k++) {
                const float ra = pb[k] / (float) la, rb = pc[k] / (float) lb;
                same = fabsf(ra - rb) < 0.12f;
            }
            pops += same;
        }
    }
    // its first and a late frame as pngs, to see what it looks at
    if (zox_tst_flyby_frames == 0 || zox_tst_flyby_frames == 40) {
        zox_save_png(zox_tst_path(zox_tst_flyby_frames ? "frame_40.png" : "frame_0.png"), c, size.x, size.y);
    }
    zox_tst_flyby_blinks[zox_tst_flyby_frames] = blinks;
    zox_tst_flyby_pops[zox_tst_flyby_frames] = pops;
    if (blinks) {
        zox_tst_flyby_keep(blinks, mask);
    }
    free(mask);
    zox_tst_flyby_frames++;
}

void zox_tst_lod_flyby_setup() {
    zox_tst_world_setup();
    const char* speed = getenv("zox_tst_flyby_speed");
    if (speed && speed[0]) {
        zox_tst_flyby_speed = atof(speed);
    }
}

byte zox_tst_lod_flyby_ready(ecs* world) {
    const entity terrain = zox_tst_world_ready(world);
    if (!terrain) {
        return 0;
    }
    const float chunk = octree_size(zox_getv(terrain, NodeDepth)) * zox_getv(terrain, BlockScale);
    if (zox_tst_flyby_stage == 0) {
        // no player (zox_tst_world_setup): its own camera, a streamer as a player's camera is (the lods round it, the
        //  chunks culled to its view: only a streaming camera culls them, the menu's culled 224 of 315 it looked at)
        zox_tst_flyby_camera = spawn_viewport_camera(world, main_app, float3_zero, quaternion_identity, 70, viewport_grid_cell(0, 1, 1));
        prefab_add_streamer(world, zox_tst_flyby_camera);
        zox_set_link(world, zox_tst_flyby_camera, StreamLink, terrain);
        zox_setv(zox_tst_flyby_camera, StreamerLevel, 1);
        zox_tst_flyby_streamer = zox_tst_flyby_camera;
        zox_tst_flyby_place(world, terrain, chunk, 0, 1);
        zox_tst_flyby_time = get_time_ms();
        zox_tst_flyby_stage = 1;
        return 0;
    }
    if (zox_tst_flyby_stage == 1) {
        zox_tst_waiting = "the land under the camera to load";
        zox_tst_flyby_settled = zox_tst_terrain_busy(world, terrain) ? 0 : zox_tst_flyby_settled + 1;
        zox_tst_progress(zox_tst_flyby_settled);
        if (get_time_ms() - zox_tst_flyby_time < 100 || zox_tst_flyby_settled < 3) {
            return 0;
        }
        // what draws, by lod: chunks, ones with a mesh shown, ones the renderer draws (shown, faces, not render disabled)
        int chunks[8] = { 0 }, shown[8] = { 0 }, drawn[8] = { 0 }, render_disabled[8] = { 0 };
        iter it = zox_children(world, terrain);
        while (zox_children_next(it)) {
            for (int j = 0; j < it.count; j++) {
                const entity c = it.entities[j];
                if (!zox_has(c, Chunk3)) {
                    continue;
                }
                const byte d = zox_getv(c, RenderDepth) & 7;
                chunks[d]++;
                render_disabled[d] += zox_getv(c, RenderDisabled);
                entity meshes[zox_chunk_meshes_max];
                const uint count = get_chunk_meshes(world, c, meshes);
                byte any_shown = 0, any_drawn = 0;
                for (uint k = 0; k < count; k++) {
                    const entity m = meshes[k];
                    if (zox_has(m, Disabled)) {
                        continue;
                    }
                    any_shown = 1;
                    any_drawn |= !zox_getv(m, RenderDisabled) && zox_getv(m, MeshRenderCount);
                }
                shown[d] += any_shown;
                drawn[d] += any_drawn;
            }
        }
        for (byte d = 0; d < 8; d++) {
            if (chunks[d]) {
                zox_log("[tst] lod_flyby: depth [%i] chunks [%i] a mesh shown [%i] drawn [%i] render disabled [%i]",
                    d, chunks[d], shown[d], drawn[d], render_disabled[d]);
            }
        }
        const char* fps_text = getenv("zox_tst_gif_fps");
        const float fps = fps_text && fps_text[0] ? atof(fps_text) : 30;
        zox_tst_flyby_place(world, terrain, chunk, 0, 1);
        zox_gif_record(zox_tst_path("flyby.gif"), fps);
        zox_log("[tst] lod_flyby: loaded, flying [%.0f] chunks at [%.1f] a second", zox_tst_flyby_length(), zox_tst_flyby_speed);
        zox_tst_flyby_stage = 2;
        return 0;
    }
    zox_tst_flyby_frame();
    const float length = zox_tst_flyby_length();
    zox_tst_waiting = "the flight";
    zox_tst_progress(zox_tst_flyby_travelled * 100);
    if (zox_tst_flyby_travelled >= length) {
        zox_gif_stop();
        return 1;
    }
    // frame rate independent: chunks a second
    zox_tst_flyby_travelled += zox_tst_flyby_speed * zox_delta_time;
    if (zox_tst_flyby_travelled > length) {
        zox_tst_flyby_travelled = length;
    }
    zox_tst_flyby_place(world, terrain, chunk, zox_tst_flyby_travelled, 0);
    return 0;
}

void zox_tst_lod_flyby_check(ecs* world) {
    (void) world;
    const int frames = zox_tst_flyby_frames;
    long blinks = 0, pops = 0;
    int most_blinks = 0, most_pops = 0;
    for (int i = 0; i < frames; i++) {
        blinks += zox_tst_flyby_blinks[i];
        pops += zox_tst_flyby_pops[i];
        most_blinks = int_max(most_blinks, zox_tst_flyby_blinks[i]);
        most_pops = int_max(most_pops, zox_tst_flyby_pops[i]);
    }
    const float pixels = (float) zox_tst_flyby_size.x * zox_tst_flyby_size.y;
    const float blink_average = frames ? blinks / (float) frames : 0;
    const float pop_average = frames ? pops / (float) frames : 0;
    zox_log("[tst] [metric] lod_flyby [%s meshing] [%i] frames [%ix%i]: blinks a frame [%.0f] (%.3f%%) most [%i], "
        "light pops a frame [%.0f] (%.3f%%) most [%i]",
        zox_chunks3_gpu_meshing ? "gpu" : "cpu", frames, zox_tst_flyby_size.x, zox_tst_flyby_size.y,
        blink_average, 100 * blink_average / pixels, most_blinks, pop_average, 100 * pop_average / pixels, most_pops);
    // the frames that stand out
    char text[512];
    int n = 0, listed = 0;
    for (int i = 0; i < frames && n < 480; i++) {
        const byte blink = zox_tst_flyby_blinks[i] > 4 * blink_average + 50;
        const byte pop = zox_tst_flyby_pops[i] > 4 * pop_average + 50;
        if (blink || pop) {
            n += snprintf(text + n, sizeof(text) - n, " [%i: %i %i]", i, zox_tst_flyby_blinks[i], zox_tst_flyby_pops[i]);
            listed++;
        }
    }
    zox_log("[tst] lod_flyby: [%i] frames stand out [frame: blinks pops]:%s", listed, listed ? text : " none");
    for (int i = 0; i < zox_tst_flyby_worst; i++) {
        zox_tst_flyby_saved* saved = &zox_tst_flyby_saves[i];
        if (!saved->image) {
            continue;
        }
        char name[64];
        snprintf(name, sizeof(name), "blink_%i.png", i);
        const char* path = zox_tst_path(name);
        zox_save_png(path, saved->image, zox_tst_flyby_size.x / 2 * 3, zox_tst_flyby_size.y / 2);
        zox_log("[tst] lod_flyby: [%s] frame [%i], [%i] pixels blinked", path, saved->frame, saved->blinks);
    }
    zox_tst_check(frames > 20, "only [%i] frames recorded", frames);
    zox_tst_check(zox_tst_flyby_travelled >= zox_tst_flyby_length(), "the flight stopped at [%.1f] chunks", zox_tst_flyby_travelled);
}

void zox_tsts_lod_flyby(ecs* world) {
    (void) world;
    zox_tst_add("lod_flyby", zox_tst_lod_flyby_ready, zox_tst_lod_flyby_check, 400);
    zox_tst_set_setup("lod_flyby", zox_tst_lod_flyby_setup, 1);
}
