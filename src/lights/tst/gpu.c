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

#define zox_tst_gpu_light_depth 5

// a terrain chunk: hills, a cave under them, sunlight down open columns, a torch in the cave, light spreading through
//  air losing light_air_decay a step (as flood fills do, simpler), solids darklight
static inline void zox_tst_gpu_light_scene(VoxelNode* voxels, LightNode* lights, byte depth) {
    const int length = octree_size(depth);
    const int cells = length * length * length;
    byte* solid = calloc(cells, 1);
    byte* light = malloc(cells);
    int* queue = malloc(sizeof(int) * cells * 7);
    #define zox_tst_gpu_cell(x, y, z) (((x) * length + (y)) * length + (z))
    create_VoxelNode(voxels);
    for (int x = 0; x < length; x++) for (int z = 0; z < length; z++) {
        const int height = (int) (length * (0.55f + 0.18f * sinf(x * 0.35f) + 0.12f * cosf(z * 0.27f + x * 0.1f)));
        for (int y = 0; y < height && y < length; y++) {
            // the cave: a tunnel under the hills, open to the front (+z), into the middle
            const float cx = length * 0.5f, cy = length * 0.3f;
            const float dx = (x - cx) / (length * 0.2f), dy = (y - cy) / (length * 0.16f);
            if (dx * dx + dy * dy < 1 && z > length * 0.35f) {
                continue;
            }
            const byte v = y < height - 4 ? 3 : (y < height - 1 ? 2 : 1);
            set_VoxelNode(voxels, depth, (byte3) { x, y, z }, v);
            solid[zox_tst_gpu_cell(x, y, z)] = 1;
        }
    }
    optimize_reduce_VoxelNode(voxels);
    int head = 0, tail = 0;
    for (int i = 0; i < cells; i++) {
        light[i] = darklight;
    }
    // sunlight down each column until it hits something
    for (int x = 0; x < length; x++) for (int z = 0; z < length; z++) {
        for (int y = length - 1; y >= 0 && !solid[zox_tst_gpu_cell(x, y, z)]; y--) {
            light[zox_tst_gpu_cell(x, y, z)] = sunlight;
            queue[tail++] = zox_tst_gpu_cell(x, y, z);
        }
    }
    // a torch in the cave
    const int torch = zox_tst_gpu_cell(length / 2, (int) (length * 0.3f), (int) (length * 0.55f));
    if (!solid[torch]) {
        light[torch] = 240;
        queue[tail++] = torch;
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
    free(solid);
    free(light);
    free(queue);
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
