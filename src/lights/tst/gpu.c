// tst: gpu_mesh_lights, gpu meshing's corner lights are smooth lighting's (chunks3/fun/gpu_mesh.c, lights3/fun/gpu.c)
//  - every corner of every gpu face against smooth_face_lights for the same voxel, face and corner, one chunk alone
//      (past it: sunlight above, darklight elsewhere, as smooth lighting has a missing chunk)
//  - cases: smooth_lights' corner (two solid edges hide a bright corner), and a terrain chunk: hills in sunlight, a
//      torch lit cave under them, its light tree reduced (big nodes), at depth 5 and two lods (coarse reads)
//  - each vertex's face lights (all 4 corners, for blending across the face) match too
//  - logs [tst] [metric]: lit vs unlit gpu mesh, the cpu's smooth lights for the same faces
//  - run: bash bsh/tst.sh gpu_mesh_lights
// tst: gpu_light_views, the terrain chunk drawn 4 ways to compare the look (bash bsh/gpu_lights.sh, tst/gpu_light_views.png)
//  - 4 copies, the same close up into each one's torch lit cave: unlit (top left, shades only), a face's light (top
//      right, flat), each corner's light blended across the face's two triangles (bottom left, vertex, as cpu meshes
//      are lit), the 4 corner lights blended across the whole face (bottom right, pixel, bilinear)
//  - border faces culled except the top, as cpu sides do with no neighbour chunk (edge_faces 0)

// tst: gpu_light_stress, terrain chunks re-lit (and re-shaped) every frame, meshed lit on the gpu, to see the update rate
//  - bash bsh/gpu_light_stress.sh (--visible to watch), tst/gpu_light_stress.png
//  - night (a dim sky), each chunk two torches moving: one walking the cave in and out, one roaming over the hills
//  - each frame, each chunk: its hills roll (voxel octree rebuilt, zox_tst_gpu_voxels=0 to keep them still), its
//      lights flood again (light octree rebuilt), both flattened, uploaded and meshed lit, drawn with pixel light
//  - zox_tst_gpu_chunks=<n> chunks (default 4, up to 16, the first 4 in view), zox_tst_gpu_seconds=<s> (default 6)
//  - logs [tst] [metric] each second: chunks updated a second, fps, cpu voxels, cpu lights, flattening (a chunk), the gpu
//      (one chunk, waited for)
//  - checks: chunks updated every second, the last frame's corner lights match smooth_face_lights (chunk 0)

#define zox_tst_gpu_light_depth 5

// a terrain chunk's voxels at time: hills (rolling with time), a cave under them open to the front (+z)
//  - solid: length^3 bytes, set where a voxel is (x, y, z order), for the light flood
static inline void zox_tst_gpu_scene_voxels(VoxelNode* voxels, byte depth, float time, byte* solid) {
    const int length = octree_size(depth);
    memset(solid, 0, length * length * length);
    create_VoxelNode(voxels);
    for (int x = 0; x < length; x++) for (int z = 0; z < length; z++) {
        const int height = (int) (length * (0.55f + 0.18f * sinf(x * 0.35f + time * 1.5f) + 0.12f * cosf(z * 0.27f + x * 0.1f + time)));
        for (int y = 0; y < height && y < length; y++) {
            // the cave: a tunnel under the hills, open to the front (+z), into the middle
            const float cx = length * 0.5f, cy = length * 0.3f;
            const float dx = (x - cx) / (length * 0.2f), dy = (y - cy) / (length * 0.16f);
            if (dx * dx + dy * dy < 1 && z > length * 0.35f) {
                continue;
            }
            const byte v = y < height - 4 ? 3 : (y < height - 1 ? 2 : 1);
            set_VoxelNode(voxels, depth, (byte3) { x, y, z }, v);
            solid[(x * length + y) * length + z] = 1;
        }
    }
    optimize_reduce_VoxelNode(voxels);
}

// a chunk's lights: sky light down each open column, torches, spreading through air losing light_air_decay a step
//  (as flood fills do, simpler), solids darklight
static inline void zox_tst_gpu_scene_lights(LightNode* lights, byte depth, const byte* solid, byte sky, const int3* torches, int torches_count, byte torch_light) {
    const int length = octree_size(depth);
    const int cells = length * length * length;
    byte* light = malloc(cells);
    int* queue = malloc(sizeof(int) * cells * 7);
    #define zox_tst_gpu_cell(x, y, z) (((x) * length + (y)) * length + (z))
    int head = 0, tail = 0;
    memset(light, darklight, cells);
    for (int x = 0; x < length; x++) for (int z = 0; z < length; z++) {
        for (int y = length - 1; y >= 0 && !solid[zox_tst_gpu_cell(x, y, z)]; y--) {
            light[zox_tst_gpu_cell(x, y, z)] = sky;
            queue[tail++] = zox_tst_gpu_cell(x, y, z);
        }
    }
    for (int t = 0; t < torches_count; t++) {
        const int3 p = torches[t];
        if (p.x < 0 || p.y < 0 || p.z < 0 || p.x >= length || p.y >= length || p.z >= length) {
            continue;
        }
        const int i = zox_tst_gpu_cell(p.x, p.y, p.z);
        if (!solid[i] && light[i] < torch_light) {
            light[i] = torch_light;
            queue[tail++] = i;
        }
    }
    while (head < tail && tail < cells * 7 - 6) {
        const int i = queue[head++];
        const int x = i / (length * length), y = (i / length) % length, z = i % length;
        const int next = light[i] - light_air_decay;
        if (next <= darklight) {
            continue;
        }
        for (byte d = 0; d < 6; d++) {
            const int3 n = int3_add((int3) { x, y, z }, get_direction_int3(d));
            if (n.x < 0 || n.y < 0 || n.z < 0 || n.x >= length || n.y >= length || n.z >= length) {
                continue;
            }
            const int j = zox_tst_gpu_cell(n.x, n.y, n.z);
            if (!solid[j] && light[j] < next) {
                light[j] = next;
                queue[tail++] = j;
            }
        }
    }
    octree_create(lights, sizeof(LightNode), darklight);
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        const byte l = light[zox_tst_gpu_cell(x, y, z)];
        if (l != darklight) {
            set_LightNode(lights, depth, (byte3) { x, y, z }, l);
        }
    }
    reduce_LightNode(lights);
    #undef zox_tst_gpu_cell
    free(light);
    free(queue);
}

// the static terrain chunk: hills in sunlight, a torch in the cave
static inline void zox_tst_gpu_light_scene(VoxelNode* voxels, LightNode* lights, byte depth) {
    const int length = octree_size(depth);
    byte* solid = malloc(length * length * length);
    zox_tst_gpu_scene_voxels(voxels, depth, 0, solid);
    const int3 torch = { length / 2, (int) (length * 0.3f), (int) (length * 0.55f) };
    zox_tst_gpu_scene_lights(lights, depth, solid, sunlight, &torch, 1, 240);
    free(solid);
}

// smooth_lights' corner case, on a depth 2 chunk: a floor voxel, two solid edges, a bright corner between them
static inline void zox_tst_gpu_light_corner(VoxelNode* voxels, LightNode* lights) {
    const byte depth = 2;
    octree_create(voxels, sizeof(VoxelNode), 0);
    octree_create(lights, sizeof(LightNode), darklight);
    set_VoxelNode(voxels, depth, (byte3) { 1, 1, 1 }, 1);
    set_VoxelNode(voxels, depth, (byte3) { 0, 2, 1 }, 1);
    set_VoxelNode(voxels, depth, (byte3) { 1, 2, 0 }, 1);
    const byte3 lit[] = { { 1, 2, 1 }, { 2, 2, 1 }, { 1, 2, 2 }, { 2, 2, 2 }, { 2, 2, 0 }, { 0, 2, 2 } };
    for (byte i = 0; i < sizeof(lit) / sizeof(lit[0]); i++) {
        set_LightNode(lights, depth, lit[i], 100);
    }
    set_LightNode(lights, depth, (byte3) { 0, 2, 0 }, 255);
}

// the cpu's vertex order on a face (smooth_vertex_signs: --, -+, ++, +-) for a gpu corner side (bit 0 +a, bit 1 +b)
static const byte zox_tst_gpu_side_vertex[4] = { 0, 3, 1, 2 };

// meshes voxels and lights on the gpu at depth, checks every corner light against smooth_face_lights
static inline void zox_tst_gpu_light_compare(GpuMesher* mesher, const VoxelNode* voxels, const LightNode* lights, const byte* solidity, byte depth, const char* name) {
    uint* nodes = NULL;
    uint* light_nodes = NULL;
    const uint count = flatten_VoxelNode(voxels, &nodes);
    const uint light_count = flatten_LightNode(lights, &light_nodes);
    const GpuMeshLighting lighting = gpu_mesh_lighting(solidity);
    gpu_mesh_flat_lit(mesher, nodes, count, light_nodes, light_count, depth, &lighting);
    free(nodes);
    free(light_nodes);
    const uint faces = gpu_mesher_read_faces(mesher);
    zox_tst_check(faces, "[%s] depth [%i] has no gpu faces", name, depth);
    float4* vertices = malloc(sizeof(float4) * 4 * (faces ? faces : 1));
    uint* packed = malloc(sizeof(uint) * 4 * (faces ? faces : 1));
    if (faces) {
        zox_gpu_storage_read(mesher->vertices, 0, sizeof(float4) * 4 * faces, vertices);
        zox_gpu_storage_read(mesher->vertex_lights, 0, sizeof(uint) * 4 * faces, packed);
    }
    // the cpu's view: this chunk alone in the middle of its 27
    const VoxelNode* voxels27[27] = { NULL };
    const LightNode* lights27[27] = { NULL };
    spinlock lock_values[54];
    spinlock* voxels_locks[27];
    spinlock* lights_locks[27];
    for (byte i = 0; i < 27; i++) {
        spinlock_init(&lock_values[i]);
        spinlock_init(&lock_values[27 + i]);
        voxels_locks[i] = &lock_values[i];
        lights_locks[i] = &lock_values[27 + i];
    }
    voxels27[OCTREE_INDEX(0, 0, 0)] = voxels;
    lights27[OCTREE_INDEX(0, 0, 0)] = lights;
    int wrong = 0, wrong_packed = 0, corners = 0, darkest = 255, brightest = 0;
    double cpu_ms = 0;
    for (uint f = 0; f < faces; f++) {
        const float4* v = &vertices[f * 4];
        const int w0 = (int) v[0].w;
        const byte d = gpu_mesh_vertex_direction(w0);
        // the voxel: the corners' smallest point, less one on its axis for a positive face
        float3 low = { v[0].x, v[0].y, v[0].z };
        for (byte c = 1; c < 4; c++) {
            low.x = fminf(low.x, v[c].x);
            low.y = fminf(low.y, v[c].y);
            low.z = fminf(low.z, v[c].z);
        }
        int3 p = { (int) low.x, (int) low.y, (int) low.z };
        if (d & 1) {
            if ((d >> 1) == 0) p.x--; else if ((d >> 1) == 1) p.y--; else p.z--;
        }
        byte expected[4];
        const double start = get_time_ms();
        smooth_face_lights(lights27, lights_locks, voxels27, voxels_locks, solidity, int3_to_byte3(p), depth, d, expected);
        cpu_ms += get_time_ms() - start;
        for (byte c = 0; c < 4; c++) {
            const int w = (int) v[c].w;
            const byte side = gpu_mesh_vertex_side(w);
            const byte light = gpu_mesh_vertex_light(w);
            const byte want = expected[zox_tst_gpu_side_vertex[side]];
            corners++;
            darkest = light < darkest ? light : darkest;
            brightest = light > brightest ? light : brightest;
            if (light != want) {
                if (!wrong) {
                    zox_tst_check(0, "[%s] depth [%i] voxel [%ix%ix%i] face [%i] corner side [%i]: gpu light [%i], cpu [%i] (cpu corners [%i %i %i %i])",
                        name, depth, p.x, p.y, p.z, d, side, light, want, expected[0], expected[1], expected[2], expected[3]);
                }
                wrong++;
            }
            if (((packed[f * 4 + c] >> (8 * side)) & 255) != light) {
                wrong_packed++;
            }
        }
    }
    zox_tst_check(!wrong, "[%s] depth [%i] [%i] of [%i] corner lights differ from smooth lighting", name, depth, wrong, corners);
    zox_tst_check(!wrong_packed, "[%s] depth [%i] [%i] face lights don't hold their corner's light", name, depth, wrong_packed);
    zox_log("[tst] [%s] depth [%i]: [%u] faces, [%i] corner lights match smooth lighting, darkest [%i] brightest [%i] (cpu smooth lights [%.2fms])",
        name, depth, faces, corners - wrong, darkest, brightest, cpu_ms);
    free(vertices);
    free(packed);
}

byte zox_tst_gpu_mesh_lights_ready(ecs* world) {
    (void) world;
    zox_tst_waiting = "a compute capable gl context (OpenGL ES 3.1+ or OpenGL 4.3+)";
    return zox_gpu_compute_supported();
}

void zox_tst_gpu_mesh_lights_check(ecs* world) {
    (void) world;
    const byte depth = zox_tst_gpu_light_depth;
    const int length = octree_size(depth);
    GpuMesher mesher;
    zox_tst_check(gpu_mesher_create(&mesher, length * length * length * 3), "the gpu mesher couldn't be made");
    if (!mesher.program) {
        return;
    }
    byte solidity[zox_light_solidity_length];
    memset(solidity, 0, zox_light_solidity_length);
    solidity[0] = solidity[1] = solidity[2] = 1;
    VoxelNode voxels;
    LightNode lights;
    zox_tst_gpu_light_corner(&voxels, &lights);
    zox_tst_gpu_light_compare(&mesher, &voxels, &lights, solidity, 2, "corner");
    close_VoxelNode(&voxels);
    close_LightNode(&lights);
    zox_tst_gpu_light_scene(&voxels, &lights, depth);
    for (int d = depth; d >= depth - 2; d--) {
        zox_tst_gpu_light_compare(&mesher, &voxels, &lights, solidity, d, "terrain");
    }
    // times: unlit and lit gpu meshes (flattening included), waited for
    const int runs = 20;
    const GpuMeshLighting lighting = gpu_mesh_lighting(solidity);
    double start = get_time_ms();
    for (int r = 0; r < runs; r++) {
        gpu_mesh_VoxelNode(&mesher, &voxels, depth);
        zox_gpu_finish();
    }
    const double unlit_ms = (get_time_ms() - start) / runs;
    start = get_time_ms();
    for (int r = 0; r < runs; r++) {
        uint* nodes = NULL;
        uint* light_nodes = NULL;
        const uint count = flatten_VoxelNode(&voxels, &nodes);
        const uint light_count = flatten_LightNode(&lights, &light_nodes);
        gpu_mesh_flat_lit(&mesher, nodes, count, light_nodes, light_count, depth, &lighting);
        zox_gpu_finish();
        free(nodes);
        free(light_nodes);
    }
    const double lit_ms = (get_time_ms() - start) / runs;
    zox_log("[tst] [metric] gpu_mesh_lights terrain depth [%i]: gpu unlit [%.3fms], lit [%.3fms] (both with flattening)", depth, unlit_ms, lit_ms);
    close_VoxelNode(&voxels);
    close_LightNode(&lights);
    gpu_mesher_dispose(&mesher);
}

// # gpu_light_views

zox_tag(GpuLightViews);
extern char zox_screenshot_request[256];
extern byte nohud;
byte zox_tst_gpu_light_views_stage = 0;
GpuMesher zox_tst_gpu_light_mesher;
uint zox_tst_gpu_light_program = 0;
const float3 zox_tst_gpu_light_spot = { 0, 1000, 0 };  // the void, past the menu's sky
const color_rgb zox_tst_gpu_light_colors[3] = { { 96, 200, 72 }, { 150, 104, 64 }, { 140, 140, 150 } };  // grass, dirt, stone
const byte zox_tst_gpu_light_modes[4] = { gpu_mesh_light_none, gpu_mesh_light_face, gpu_mesh_light_vertex, gpu_mesh_light_pixel };
const char* zox_tst_gpu_light_mode_names[4] = { "unlit", "face", "vertex", "pixel" };

// where copy i of the chunk sits (its corner): a row along x, far apart so each close up sees its own
static inline float3 zox_tst_gpu_light_position(int i) {
    return (float3) { zox_tst_gpu_light_spot.x + i * 20.0f, zox_tst_gpu_light_spot.y, zox_tst_gpu_light_spot.z };
}

byte zox_tst_gpu_light_views_ready(ecs* world) {
    zox_tst_progress(zox_tst_gpu_light_views_stage);
    if (zox_tst_gpu_light_views_stage == 0) {
        zox_tst_waiting = "the app and a compute capable gl context";
        if (!zox_valid(main_app) || !zox_valid(main_cameras[0]) || !zox_gpu_compute_supported()) {
            return 0;
        }
        const byte depth = zox_tst_gpu_light_depth;
        const int length = octree_size(depth);
        zox_tst_check(gpu_mesher_create(&zox_tst_gpu_light_mesher, length * length * length * 3), "the gpu mesher couldn't be made");
        zox_tst_gpu_light_program = gpu_mesh_draw_program();
        zox_tst_check(zox_tst_gpu_light_program, "the gpu mesh draw program didn't compile");
        if (!zox_tst_gpu_light_program || !zox_tst_gpu_light_mesher.program) {
            return 1;
        }
        byte solidity[zox_light_solidity_length];
        memset(solidity, 0, zox_light_solidity_length);
        solidity[0] = solidity[1] = solidity[2] = 1;
        VoxelNode voxels;
        LightNode lights;
        zox_tst_gpu_light_scene(&voxels, &lights, depth);
        uint* nodes = NULL;
        uint* light_nodes = NULL;
        const uint count = flatten_VoxelNode(&voxels, &nodes);
        const uint light_count = flatten_LightNode(&lights, &light_nodes);
        const GpuMeshLighting lighting = gpu_mesh_lighting(solidity);
        zox_tst_gpu_light_mesher.edge_faces = 0;
        gpu_mesh_flat_lit(&zox_tst_gpu_light_mesher, nodes, count, light_nodes, light_count, depth, &lighting);
        free(nodes);
        free(light_nodes);
        close_VoxelNode(&voxels);
        close_LightNode(&lights);
        // the same close up into each copy's cave mouth (the chunk is 2 units, the mouth at its front, low)
        for (int i = 0; i < 4; i++) {
            const float3 mouth = float3_add(zox_tst_gpu_light_position(i), (float3) { 1, 0.6f, 2 });
            const float3 target = float3_add(mouth, (float3) { 0, -0.05f, -0.7f });
            const float3 eye = float3_add(mouth, (float3) { 0.45f, 0.35f, 1.1f });
            const entity camera = spawn_viewport_camera(world, main_app, eye, camera_look_at(eye, target), 60, viewport_grid_cell(i, 2, 2));
            zox_setv(camera, Color, ((color) { 96, 96, 96, 255 }));
        }
        nohud = 1;
        const entity game_view = zox_get_link(world, main_cameras[0], RenderTextureLink);
        if (zox_valid(game_view)) {
            zox_setv(game_view, RenderDisabled, 1);
        }
        const entity e = zox_new();
        zox_add(e, GpuLightViews);
        zox_tst_gpu_light_views_stage = 1;
        return 0;
    }
    if (zox_tst_gpu_light_views_stage == 1) {
        // drawn once its cameras are placed: the screenshot is taken at the end of a frame they drew in
        zox_tst_waiting = "the light views drawn";
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("gpu_light_views.ppm"));
        zox_tst_gpu_light_views_stage = 2;
        return 0;
    }
    zox_tst_waiting = "the light views' screenshot";
    return !zox_screenshot_request[0];
}

void zox_tst_gpu_light_views_check(ecs* world) {
    (void) world;
    int2 size;
    byte* pixels = zox_tst_load_ppm(zox_tst_path("gpu_light_views.ppm"), &size);
    zox_tst_check(pixels, "no screenshot [%s]", zox_tst_path("gpu_light_views.ppm"));
    if (pixels) {
        // each view (one copy) drew it: pixels not the gray background, and their brightness
        const byte gray[3] = { 96, 96, 96 };
        int brightness[4] = { 0 }, drawn[4] = { 0 };
        for (int y = 0; y < size.y; y++) {
            for (int x = 0; x < size.x; x++) {
                const byte* p = zox_tst_ppm_pixel(pixels, size, x, y);
                if (zox_tst_pixel_difference(p, gray) <= 24) {
                    continue;
                }
                const int view = (x * 2 / size.x) + (y * 2 / size.y) * 2;
                drawn[view]++;
                brightness[view] += p[0] + p[1] + p[2];
            }
        }
        // how far vertex and pixel lighting differ: the same view, only the blend across a face changes
        int differ = 0, compared = 0;
        const int2 half = { size.x / 2, size.y / 2 };
        for (int y = 0; y < half.y; y++) {
            for (int x = 0; x < half.x; x++) {
                const byte* vertex = zox_tst_ppm_pixel(pixels, size, x, half.y + y);
                const byte* pixel = zox_tst_ppm_pixel(pixels, size, half.x + x, half.y + y);
                if (zox_tst_pixel_difference(vertex, gray) <= 24 && zox_tst_pixel_difference(pixel, gray) <= 24) {
                    continue;
                }
                compared++;
                differ += zox_tst_pixel_difference(vertex, pixel) > 6;
            }
        }
        zox_log("[tst] [metric] gpu_light_views: vertex and pixel lighting differ on [%.1f%%] of the drawn pixels", compared ? 100.0f * differ / compared : 0);
        zox_tst_check(differ, "vertex and pixel lighting look the same, the bilinear blend didn't run");
        for (int i = 0; i < 4; i++) {
            const int average = drawn[i] ? brightness[i] / drawn[i] : 0;
            zox_log("[tst] copy [%s]: [%i] pixels, brightness [%i]", zox_tst_gpu_light_mode_names[i], drawn[i], average);
            zox_tst_check(drawn[i], "copy [%s] wasn't drawn", zox_tst_gpu_light_mode_names[i]);
        }
        free(pixels);
    }
    gpu_mesher_dispose(&zox_tst_gpu_light_mesher);
}

// draws the 4 copies, each its own light mode, in each 3D camera's pass while the test runs
void gpu_light_views_render_system(iter* it) {
    zox_sys_on_begin();
    if (!zox_tst_gpu_light_program) {
        return;
    }
    const float scale = 2.0f / octree_size(zox_tst_gpu_light_depth);
    for (int i = 0; i < 4; i++) {
        gpu_mesher_draw(&zox_tst_gpu_light_mesher, zox_tst_gpu_light_program, render_camera_matrix,
            zox_tst_gpu_light_position(i), scale, zox_tst_gpu_light_colors, 3, zox_tst_gpu_light_modes[i]);
    }
    zox_sys_on_end();
} zoxd_system(gpu_light_views_render_system);

void zox_tsts_lights_gpu(ecs* world) {
    zoxd_tag(GpuLightViews);
    zox_tst_add("gpu_mesh_lights", zox_tst_gpu_mesh_lights_ready, zox_tst_gpu_mesh_lights_check, 60);
    zox_tst_set_setup("gpu_mesh_lights", NULL, 1);
    zox_tst_add("gpu_light_views", zox_tst_gpu_light_views_ready, zox_tst_gpu_light_views_check, 60);
    zox_tst_set_setup("gpu_light_views", NULL, 1);
    zox_render3_system(2,
        gpu_light_views_render_system,
        [none] lights.GpuLightViews
    );
}



// # gpu_light_stress

zox_tag(GpuLightStress);
#define zox_tst_gpu_stress_max 16
#define zox_tst_gpu_stress_sky 56          // night: a dim sky, so torches show on the hills
#define zox_tst_gpu_stress_torch 255
byte zox_tst_gpu_stress_stage = 0;
int zox_tst_gpu_stress_count = 0;
byte zox_tst_gpu_stress_voxels = 1;        // the hills roll, else only lights change
GpuMesher zox_tst_gpu_stress_meshers[zox_tst_gpu_stress_max];
uint zox_tst_gpu_stress_program = 0;
byte zox_tst_gpu_stress_solidity[zox_light_solidity_length];
byte* zox_tst_gpu_stress_solid[zox_tst_gpu_stress_max];         // each chunk's solid cells, for its light flood
uint* zox_tst_gpu_stress_nodes[zox_tst_gpu_stress_max];         // each chunk's flat voxels (kept when they don't change)
uint zox_tst_gpu_stress_nodes_count[zox_tst_gpu_stress_max];
VoxelNode zox_tst_gpu_stress_last_voxels;   // chunk 0's last frame, checked at the end
LightNode zox_tst_gpu_stress_last_lights;
byte zox_tst_gpu_stress_has_last = 0;
double zox_tst_gpu_stress_start = 0;
double zox_tst_gpu_stress_seconds = 6;
double zox_tst_gpu_stress_second_start = 0;
int zox_tst_gpu_stress_second_updates = 0;
int zox_tst_gpu_stress_second_frames = 0;
double zox_tst_gpu_stress_ms_voxels = 0, zox_tst_gpu_stress_ms_lights = 0, zox_tst_gpu_stress_ms_flatten = 0, zox_tst_gpu_stress_ms_gpu = 0;
int zox_tst_gpu_stress_seconds_logged = 0;
int zox_tst_gpu_stress_seconds_updated = 0;

static inline float3 zox_tst_gpu_stress_position(int i) {
    return (float3) { zox_tst_gpu_light_spot.x + i * 20.0f, zox_tst_gpu_light_spot.y, zox_tst_gpu_light_spot.z + 40 };
}

// chunk i's two torches at time: one walking the cave in and out, one circling over the hills
static inline void zox_tst_gpu_stress_torches(int i, double time, byte depth, const byte* solid, int3* torches) {
    const int length = octree_size(depth);
    const float t = (float) time + i * 1.7f;
    torches[0] = (int3) { length / 2, (int) (length * 0.3f), (int) (length * (0.65f + 0.3f * sinf(t * 0.9f))) };
    const int x = (int) (length * (0.5f + 0.35f * cosf(t * 0.7f)));
    const int z = (int) (length * (0.5f + 0.35f * sinf(t * 0.7f)));
    // just over the ground there
    int y = length - 1;
    while (y > 0 && !solid[(x * length + (y - 1)) * length + z]) {
        y--;
    }
    torches[1] = (int3) { x, y + 1 < length ? y + 1 : length - 1, z };
}

// updates every chunk: voxels (if they roll), lights, flatten, mesh lit; logs a second's numbers once it passes
static inline void zox_tst_gpu_stress_update(ecs* world) {
    const byte depth = zox_tst_gpu_light_depth;
    const double time = zox_current_time - zox_tst_gpu_stress_start;
    const GpuMeshLighting lighting = gpu_mesh_lighting(zox_tst_gpu_stress_solidity);
    GpuMeshLighting night = lighting;
    night.sunlight = zox_tst_gpu_stress_sky;
    for (int i = 0; i < zox_tst_gpu_stress_count; i++) {
        double start = get_time_ms();
        VoxelNode voxels;
        const byte voxels_change = zox_tst_gpu_stress_voxels || !zox_tst_gpu_stress_nodes[i];
        if (voxels_change) {
            zox_tst_gpu_scene_voxels(&voxels, depth, (zox_tst_gpu_stress_voxels ? (float) time : 0) + i * 1.3f, zox_tst_gpu_stress_solid[i]);
        }
        zox_tst_gpu_stress_ms_voxels += get_time_ms() - start;
        start = get_time_ms();
        int3 torches[2];
        zox_tst_gpu_stress_torches(i, time, depth, zox_tst_gpu_stress_solid[i], torches);
        LightNode lights;
        zox_tst_gpu_scene_lights(&lights, depth, zox_tst_gpu_stress_solid[i], zox_tst_gpu_stress_sky, torches, 2, zox_tst_gpu_stress_torch);
        zox_tst_gpu_stress_ms_lights += get_time_ms() - start;
        start = get_time_ms();
        if (voxels_change) {
            free(zox_tst_gpu_stress_nodes[i]);
            zox_tst_gpu_stress_nodes_count[i] = flatten_VoxelNode(&voxels, &zox_tst_gpu_stress_nodes[i]);
        }
        uint* light_nodes = NULL;
        const uint light_count = flatten_LightNode(&lights, &light_nodes);
        zox_tst_gpu_stress_ms_flatten += get_time_ms() - start;
        if (i == 0) {
            zox_gpu_finish();
        }
        start = get_time_ms();
        gpu_mesh_flat_lit(&zox_tst_gpu_stress_meshers[i], zox_tst_gpu_stress_nodes[i], zox_tst_gpu_stress_nodes_count[i],
            light_nodes, light_count, depth, &night);
        if (i == 0) {
            zox_gpu_finish();
            zox_tst_gpu_stress_ms_gpu += get_time_ms() - start;
        }
        free(light_nodes);
        // chunk 0's frame kept for the check (its voxels rebuilt if they didn't change this frame)
        if (i == 0) {
            if (zox_tst_gpu_stress_has_last) {
                close_VoxelNode(&zox_tst_gpu_stress_last_voxels);
                close_LightNode(&zox_tst_gpu_stress_last_lights);
            }
            if (!voxels_change) {
                byte* solid = malloc(octree_size(depth) * octree_size(depth) * octree_size(depth));
                zox_tst_gpu_scene_voxels(&voxels, depth, i * 1.3f, solid);
                free(solid);
            }
            zox_tst_gpu_stress_last_voxels = voxels;
            zox_tst_gpu_stress_last_lights = lights;
            zox_tst_gpu_stress_has_last = 1;
        } else {
            if (voxels_change) {
                close_VoxelNode(&voxels);
            }
            close_LightNode(&lights);
        }
        zox_tst_gpu_stress_second_updates++;
    }
    zox_tst_gpu_stress_second_frames++;
    if (zox_current_time - zox_tst_gpu_stress_second_start >= 1) {
        const double seconds = zox_current_time - zox_tst_gpu_stress_second_start;
        const int updates = zox_tst_gpu_stress_second_updates ? zox_tst_gpu_stress_second_updates : 1;
        const int frames = zox_tst_gpu_stress_second_frames ? zox_tst_gpu_stress_second_frames : 1;
        zox_log("[tst] [metric] gpu_light_stress second [%i]: [%.0f] chunks updated a second ([%i] chunks, [%.1f] fps), a chunk: cpu voxels [%.2fms] cpu lights [%.2fms] flatten [%.2fms] gpu upload and mesh [%.2fms], [%u] faces",
            zox_tst_gpu_stress_seconds_logged, zox_tst_gpu_stress_second_updates / seconds, zox_tst_gpu_stress_count, zox_tst_gpu_stress_second_frames / seconds,
            zox_tst_gpu_stress_ms_voxels / updates, zox_tst_gpu_stress_ms_lights / updates, zox_tst_gpu_stress_ms_flatten / updates,
            zox_tst_gpu_stress_ms_gpu / frames, gpu_mesher_read_faces(&zox_tst_gpu_stress_meshers[0]));
        zox_tst_gpu_stress_seconds_updated += zox_tst_gpu_stress_second_updates > 0;
        zox_tst_gpu_stress_seconds_logged++;
        zox_tst_gpu_stress_second_start = zox_current_time;
        zox_tst_gpu_stress_second_updates = 0;
        zox_tst_gpu_stress_second_frames = 0;
        zox_tst_gpu_stress_ms_voxels = zox_tst_gpu_stress_ms_lights = zox_tst_gpu_stress_ms_flatten = zox_tst_gpu_stress_ms_gpu = 0;
    }
}

byte zox_tst_gpu_light_stress_ready(ecs* world) {
    zox_tst_progress(zox_tst_gpu_stress_stage * 100000 + zox_tst_gpu_stress_second_frames + zox_tst_gpu_stress_seconds_logged * 1000);
    if (zox_tst_gpu_stress_stage == 0) {
        zox_tst_waiting = "the app and a compute capable gl context";
        if (!zox_valid(main_app) || !zox_valid(main_cameras[0]) || !zox_gpu_compute_supported()) {
            return 0;
        }
        const char* count_text = getenv("zox_tst_gpu_chunks");
        zox_tst_gpu_stress_count = count_text && count_text[0] ? atoi(count_text) : 4;
        zox_tst_gpu_stress_count = zox_tst_gpu_stress_count < 1 ? 1 : (zox_tst_gpu_stress_count > zox_tst_gpu_stress_max ? zox_tst_gpu_stress_max : zox_tst_gpu_stress_count);
        const char* seconds_text = getenv("zox_tst_gpu_seconds");
        zox_tst_gpu_stress_seconds = seconds_text && seconds_text[0] ? atof(seconds_text) : 6;
        const char* voxels_text = getenv("zox_tst_gpu_voxels");
        zox_tst_gpu_stress_voxels = voxels_text && voxels_text[0] ? atoi(voxels_text) != 0 : 1;
        memset(zox_tst_gpu_stress_solidity, 0, zox_light_solidity_length);
        zox_tst_gpu_stress_solidity[0] = zox_tst_gpu_stress_solidity[1] = zox_tst_gpu_stress_solidity[2] = 1;
        const int length = octree_size(zox_tst_gpu_light_depth);
        for (int i = 0; i < zox_tst_gpu_stress_count; i++) {
            zox_tst_check(gpu_mesher_create(&zox_tst_gpu_stress_meshers[i], length * length * length * 3), "gpu mesher [%i] couldn't be made", i);
            zox_tst_gpu_stress_meshers[i].edge_faces = 0;
            zox_tst_gpu_stress_solid[i] = malloc(length * length * length);
            zox_tst_gpu_stress_nodes[i] = NULL;
        }
        zox_tst_gpu_stress_program = gpu_mesh_draw_program();
        zox_tst_check(zox_tst_gpu_stress_program, "the gpu mesh draw program didn't compile");
        if (!zox_tst_gpu_stress_program || !zox_tst_gpu_stress_meshers[0].program) {
            return 1;
        }
        zox_log("[tst] gpu_light_stress: [%i] chunks of [%i]^3, voxels [%s], lights every frame",
            zox_tst_gpu_stress_count, length, zox_tst_gpu_stress_voxels ? "rolling" : "still");
        // a view of each of the first 4 chunks, from the front and above
        for (int i = 0; i < 4; i++) {
            const float3 centre = float3_add(zox_tst_gpu_stress_position(i), (float3) { 1, 0.8f, 1 });
            const float3 eye = float3_add(centre, (float3) { 0.9f, 1.6f, 2.9f });
            const entity camera = spawn_viewport_camera(world, main_app, eye, camera_look_at(eye, centre), 55, viewport_grid_cell(i, 2, 2));
            zox_setv(camera, Color, ((color) { 24, 26, 40, 255 }));
        }
        nohud = 1;
        const entity game_view = zox_get_link(world, main_cameras[0], RenderTextureLink);
        if (zox_valid(game_view)) {
            zox_setv(game_view, RenderDisabled, 1);
        }
        const entity e = zox_new();
        zox_add(e, GpuLightStress);
        zox_tst_gpu_stress_start = zox_current_time;
        zox_tst_gpu_stress_second_start = zox_current_time;
        zox_tst_gpu_stress_stage = 1;
        return 0;
    }
    zox_tst_gpu_stress_update(world);
    if (zox_tst_gpu_stress_stage == 1) {
        zox_tst_waiting = "chunks re-lit and meshed on the gpu";
        if (zox_current_time - zox_tst_gpu_stress_start < zox_tst_gpu_stress_seconds) {
            return 0;
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("gpu_light_stress.ppm"));
        zox_tst_gpu_stress_stage = 2;
        return 0;
    }
    zox_tst_waiting = "the light stress screenshot";
    return !zox_screenshot_request[0];
}

void zox_tst_gpu_light_stress_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_gpu_stress_seconds_logged && zox_tst_gpu_stress_seconds_updated == zox_tst_gpu_stress_seconds_logged,
        "chunks updated in [%i] of [%i] seconds", zox_tst_gpu_stress_seconds_updated, zox_tst_gpu_stress_seconds_logged);
    // the last frame, animated voxels and moved torches, still lit as smooth lighting lights it
    if (zox_tst_gpu_stress_has_last) {
        GpuMesher* mesher = &zox_tst_gpu_stress_meshers[0];
        mesher->edge_faces = 1;
        zox_tst_gpu_light_compare(mesher, &zox_tst_gpu_stress_last_voxels, &zox_tst_gpu_stress_last_lights,
            zox_tst_gpu_stress_solidity, zox_tst_gpu_light_depth, "stress last frame");
        close_VoxelNode(&zox_tst_gpu_stress_last_voxels);
        close_LightNode(&zox_tst_gpu_stress_last_lights);
    }
    for (int i = 0; i < zox_tst_gpu_stress_count; i++) {
        gpu_mesher_dispose(&zox_tst_gpu_stress_meshers[i]);
        free(zox_tst_gpu_stress_solid[i]);
        free(zox_tst_gpu_stress_nodes[i]);
    }
}

void gpu_light_stress_render_system(iter* it) {
    zox_sys_on_begin();
    if (!zox_tst_gpu_stress_program) {
        return;
    }
    const float scale = 2.0f / octree_size(zox_tst_gpu_light_depth);
    for (int i = 0; i < zox_tst_gpu_stress_count; i++) {
        gpu_mesher_draw(&zox_tst_gpu_stress_meshers[i], zox_tst_gpu_stress_program, render_camera_matrix,
            zox_tst_gpu_stress_position(i), scale, zox_tst_gpu_light_colors, 3, gpu_mesh_light_pixel);
    }
    zox_sys_on_end();
} zoxd_system(gpu_light_stress_render_system);

void zox_tsts_lights_gpu_stress(ecs* world) {
    zoxd_tag(GpuLightStress);
    zox_tst_add("gpu_light_stress", zox_tst_gpu_light_stress_ready, zox_tst_gpu_light_stress_check, 120);
    zox_tst_set_setup("gpu_light_stress", NULL, 1);
    zox_render3_system(2,
        gpu_light_stress_render_system,
        [none] lights.GpuLightStress
    );
}
