// tst: gpu_mesh_views, vox models animated every frame and meshed on the gpu, to look at and to see how far it goes
//  - bash bsh/gpu_mesh.sh (then tst/gpu_mesh_views.png), or bash bsh/tst.sh gpu_mesh_views
//  - zox_tst_vox=<file> the vox file (res/voxes, no extension, default chicken), zox_tst_gpu_models=<n> how many
//      (default 9, in a grid), zox_tst_gpu_seconds=<s> how long it runs (default 6)
//  - each frame each model's voxels move in a wave, its octree is rebuilt (set_VoxelNode, as an edit would),
//      flattened, uploaded and meshed by the compute shader (fun/gpu_mesh.c), then drawn straight from the gpu
//      buffers by an indirect draw (gpu_mesh_views_render_system): no mesh comes back to the cpu
//  - logs [tst] [metric] lines each second: models meshed a second, the cpu's part (octree and flatten) and the gpu's
//      (one model, waited for), the frame time
//  - 4 viewports on gray round the grid (spawn_viewport_camera), the menu's view hidden, as tst body_views
//  - checks: models were meshed every second it ran, and each view drew them (its middle isn't all gray)
//  - no lights (each face direction a fixed shade), no lods, models have no neighbours
extern char zox_screenshot_request[256];
extern byte nohud;
zox_tag(GpuMeshViews);
#define zox_tst_gpu_views_count 4
#define zox_tst_gpu_models_max 64
const char* zox_tst_gpu_views_names[zox_tst_gpu_views_count] = { "+z", "+x", "-z", "above" };
const float3 zox_tst_gpu_views_spot = { 0, 1000, 0 };  // the void, past the menu's sky
byte zox_tst_gpu_views_stage = 0;
GpuMesher zox_tst_gpu_meshers[zox_tst_gpu_models_max];
uint zox_tst_gpu_draw_program = 0;
int zox_tst_gpu_models = 0;
byte zox_tst_gpu_depth = 0;            // the meshed grid's depth: the smallest the model and its wave fit in
int zox_tst_gpu_amplitude = 1;          // how far a column bobs, in voxels
int3 zox_tst_gpu_size;                  // the model's voxels
int3 zox_tst_gpu_offset;                // where it sits in the grid
byte* zox_tst_gpu_voxels = NULL;        // the model's voxels, x y z
color_rgb zox_tst_gpu_colors[256];
int zox_tst_gpu_colors_count = 0;
float zox_tst_gpu_scale = 1;            // a voxel's size in the world
double zox_tst_gpu_start = 0;
double zox_tst_gpu_seconds = 6;
// this second's numbers
double zox_tst_gpu_second_start = 0;
int zox_tst_gpu_second_updates = 0;
int zox_tst_gpu_second_frames = 0;
double zox_tst_gpu_second_cpu_ms = 0;
double zox_tst_gpu_second_gpu_ms = 0;
int zox_tst_gpu_seconds_logged = 0;
int zox_tst_gpu_seconds_meshed = 0;    // seconds that meshed at least once
uint zox_tst_gpu_faces = 0;            // the first model's faces, last read
entity zox_tst_gpu_views_entity = 0;

static inline float3 zox_tst_gpu_model_position(int i) {
    const int columns = (int) ceilf(sqrtf((float) zox_tst_gpu_models));
    const float length = octree_size(zox_tst_gpu_depth) * zox_tst_gpu_scale;
    const float gap = length * 1.15f;
    const float width = (columns - 1) * gap;
    return (float3) {
        zox_tst_gpu_views_spot.x + (i % columns) * gap - width / 2 - length / 2,
        zox_tst_gpu_views_spot.y - length / 2,
        zox_tst_gpu_views_spot.z + (i / columns) * gap - width / 2 - length / 2 };
}

// the model's voxels once, from its vox file entity
static inline byte zox_tst_gpu_load(ecs* world, entity vox) {
    if (!zox_has(vox, VoxelNode) || !zox_has(vox, NodeDepth) || !zox_has(vox, ChunkSize)) {
        return 0;
    }
    const byte depth = zox_getv(vox, NodeDepth);
    const int3 size = zox_getv(vox, ChunkSize);
    const VoxelNode* voxels = zox_get(vox, VoxelNode);
    zox_tst_gpu_size = size;
    // a ripple a sixth of its height, the grid the smallest that holds it waving
    zox_tst_gpu_amplitude = size.y / 6 > 1 ? size.y / 6 : 1;
    const int needed = int_max(int_max(size.x, size.z), size.y + 2 * zox_tst_gpu_amplitude);
    zox_tst_gpu_depth = 1;
    while (octree_size(zox_tst_gpu_depth) < needed && zox_tst_gpu_depth < 7) {
        zox_tst_gpu_depth++;
    }
    const int grid = octree_size(zox_tst_gpu_depth);
    zox_tst_gpu_offset = (int3) { (grid - size.x) / 2, (grid - size.y) / 2, (grid - size.z) / 2 };
    zox_tst_gpu_voxels = malloc(size.x * size.y * size.z);
    for (int x = 0; x < size.x; x++) for (int y = 0; y < size.y; y++) for (int z = 0; z < size.z; z++) {
        zox_tst_gpu_voxels[(x * size.y + y) * size.z + z] = getv_VoxelNode(voxels, depth, (byte3) { x, y, z });
    }
    if (zox_has(vox, ColorRGBs)) {
        const ColorRGBs* colors = zox_get(vox, ColorRGBs);
        zox_tst_gpu_colors_count = colors->length < 256 ? colors->length : 256;
        memcpy(zox_tst_gpu_colors, colors->value, sizeof(color_rgb) * zox_tst_gpu_colors_count);
    }
    zox_tst_gpu_scale = 2.0f / grid;
    return 1;
}

// model i's voxels this frame: each column bobs up and down in a wave, its own phase
static inline void zox_tst_gpu_animate(VoxelNode* node, int i, double time) {
    create_VoxelNode(node);
    const int3 size = zox_tst_gpu_size;
    const int grid = octree_size(zox_tst_gpu_depth);
    const float amplitude = zox_tst_gpu_amplitude;
    for (int x = 0; x < size.x; x++) for (int z = 0; z < size.z; z++) {
        const int lift = (int) roundf(amplitude * sinf((float) time * 4 + x * 0.3f + z * 0.2f + i * 0.9f));
        for (int y = 0; y < size.y; y++) {
            const byte v = zox_tst_gpu_voxels[(x * size.y + y) * size.z + z];
            const int3 p = { x + zox_tst_gpu_offset.x, y + zox_tst_gpu_offset.y + lift, z + zox_tst_gpu_offset.z };
            if (v && p.y >= 0 && p.y < grid) {
                set_VoxelNode(node, zox_tst_gpu_depth, (byte3) { p.x, p.y, p.z }, v);
            }
        }
    }
}

// remeshes every model, logs a second's numbers once it passes
static inline void zox_tst_gpu_update(ecs* world) {
    const double time = zox_current_time - zox_tst_gpu_start;
    for (int i = 0; i < zox_tst_gpu_models; i++) {
        const double cpu_start = get_time_ms();
        VoxelNode node;
        zox_tst_gpu_animate(&node, i, time);
        uint* nodes = NULL;
        const uint count = flatten_VoxelNode(&node, &nodes);
        collapse_VoxelNode(&node);
        zox_tst_gpu_second_cpu_ms += get_time_ms() - cpu_start;
        // one model a frame is waited for, to time the gpu's part
        if (i == 0) {
            zox_gpu_finish();
        }
        const double gpu_start = get_time_ms();
        gpu_mesh_flat(&zox_tst_gpu_meshers[i], nodes, count, zox_tst_gpu_depth);
        if (i == 0) {
            zox_gpu_finish();
            zox_tst_gpu_second_gpu_ms += get_time_ms() - gpu_start;
        }
        free(nodes);
        zox_tst_gpu_second_updates++;
    }
    zox_tst_gpu_second_frames++;
    if (zox_current_time - zox_tst_gpu_second_start >= 1) {
        const double seconds = zox_current_time - zox_tst_gpu_second_start;
        const int frames = zox_tst_gpu_second_frames;
        zox_tst_gpu_faces = gpu_mesher_read_faces(&zox_tst_gpu_meshers[0]);
        zox_log("[tst] [metric] gpu_mesh_views second [%i]: [%.0f] models meshed a second ([%i] models, [%.1f] fps), cpu octree and flatten [%.2fms] a model, gpu mesh [%.2fms] a model, [%u] faces",
            zox_tst_gpu_seconds_logged, zox_tst_gpu_second_updates / seconds, zox_tst_gpu_models, frames / seconds,
            zox_tst_gpu_second_cpu_ms / (zox_tst_gpu_second_updates ? zox_tst_gpu_second_updates : 1),
            zox_tst_gpu_second_gpu_ms / (frames ? frames : 1), zox_tst_gpu_faces);
        zox_tst_gpu_seconds_meshed += zox_tst_gpu_second_updates > 0;
        zox_tst_gpu_seconds_logged++;
        zox_tst_gpu_second_start = zox_current_time;
        zox_tst_gpu_second_updates = 0;
        zox_tst_gpu_second_frames = 0;
        zox_tst_gpu_second_cpu_ms = 0;
        zox_tst_gpu_second_gpu_ms = 0;
    }
}

byte zox_tst_gpu_mesh_views_ready(ecs* world) {
    zox_tst_progress(zox_tst_gpu_views_stage * 100000 + zox_tst_gpu_second_frames + zox_tst_gpu_seconds_logged * 1000);
    if (zox_tst_gpu_views_stage == 0) {
        zox_tst_waiting = "the app, a compute capable gl context and the vox file";
        if (!zox_valid(main_app) || !zox_valid(main_cameras[0]) || !zox_gpu_compute_supported() || !files_hashmap_voxes) {
            return 0;
        }
        const char* name = getenv("zox_tst_vox");
        name = name && name[0] ? name : "chicken";
        const entity vox = string_hashmap_get(files_hashmap_voxes, new_string_data(name));
        zox_tst_check(zox_valid(vox) && zox_tst_gpu_load(world, vox), "no vox file [%s] with voxels (res/voxes)", name);
        if (!zox_tst_gpu_voxels) {
            return 1;
        }
        const char* count_text = getenv("zox_tst_gpu_models");
        zox_tst_gpu_models = count_text && count_text[0] ? atoi(count_text) : 9;
        zox_tst_gpu_models = zox_tst_gpu_models < 1 ? 1 : (zox_tst_gpu_models > zox_tst_gpu_models_max ? zox_tst_gpu_models_max : zox_tst_gpu_models);
        const char* seconds_text = getenv("zox_tst_gpu_seconds");
        zox_tst_gpu_seconds = seconds_text && seconds_text[0] ? atof(seconds_text) : 6;
        const int grid = octree_size(zox_tst_gpu_depth);
        for (int i = 0; i < zox_tst_gpu_models; i++) {
            zox_tst_check(gpu_mesher_create(&zox_tst_gpu_meshers[i], grid * grid * grid * 3), "gpu mesher [%i] couldn't be made", i);
        }
        zox_tst_gpu_draw_program = gpu_mesh_draw_program();
        zox_tst_check(zox_tst_gpu_draw_program, "the gpu mesh draw program didn't compile");
        if (!zox_tst_gpu_draw_program || !zox_tst_gpu_meshers[0].program) {
            return 1;
        }
        zox_log("[tst] gpu_mesh_views: [%i] [%s] models, [%ix%ix%i] voxels in a [%i] grid (depth [%i])",
            zox_tst_gpu_models, name, zox_tst_gpu_size.x, zox_tst_gpu_size.y, zox_tst_gpu_size.z, grid, zox_tst_gpu_depth);
        // the views round the grid, far enough to see all of it
        const int columns = (int) ceilf(sqrtf((float) zox_tst_gpu_models));
        const float extent = columns * 2.0f * 1.15f;
        const float fov = 50;
        const float distance = extent * 0.75f / sinf(fov * 0.5f * degreesToRadians);
        const float3 directions[zox_tst_gpu_views_count] = { { 0, 0.35f, 1 }, { 1, 0.35f, 0 }, { 0, 0.35f, -1 }, { 0.5f, 1.2f, 0.5f } };
        for (int i = 0; i < zox_tst_gpu_views_count; i++) {
            const float3 position = float3_add(zox_tst_gpu_views_spot, float3_scale(float3_normalize(directions[i]), distance));
            const entity camera = spawn_viewport_camera(world, main_app, position, camera_look_at(position, zox_tst_gpu_views_spot), fov, viewport_grid_cell(i, 2, 2));
            zox_setv(camera, Color, ((color) { 96, 96, 96, 255 }));
        }
        nohud = 1;
        const entity game_view = zox_get_link(world, main_cameras[0], RenderTextureLink);
        if (zox_valid(game_view)) {
            zox_setv(game_view, RenderDisabled, 1);
        }
        // the render system draws the meshes while this exists
        zox_tst_gpu_views_entity = zox_new();
        zox_add(zox_tst_gpu_views_entity, GpuMeshViews);
        zox_tst_gpu_start = zox_current_time;
        zox_tst_gpu_second_start = zox_current_time;
        zox_tst_gpu_views_stage = 1;
        return 0;
    }
    if (zox_tst_gpu_views_stage == 1) {
        zox_tst_waiting = "models animating and meshing on the gpu";
        zox_tst_gpu_update(world);
        if (zox_current_time - zox_tst_gpu_start < zox_tst_gpu_seconds) {
            return 0;
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("gpu_mesh_views.ppm"));
        zox_tst_gpu_views_stage = 2;
        return 0;
    }
    // keeps meshing until the screenshot's saved, at the end of the frame it was asked in
    zox_tst_waiting = "the gpu mesh views' screenshot";
    zox_tst_gpu_update(world);
    return !zox_screenshot_request[0];
}

void zox_tst_gpu_mesh_views_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_gpu_seconds_logged && zox_tst_gpu_seconds_meshed == zox_tst_gpu_seconds_logged,
        "models meshed in [%i] of [%i] seconds", zox_tst_gpu_seconds_meshed, zox_tst_gpu_seconds_logged);
    zox_tst_check(zox_tst_gpu_faces, "the first model's gpu mesh has no faces");
    int2 size;
    byte* pixels = zox_tst_load_ppm(zox_tst_path("gpu_mesh_views.ppm"), &size);
    zox_tst_check(pixels, "no screenshot [%s]", zox_tst_path("gpu_mesh_views.ppm"));
    if (pixels) {
        const byte gray[3] = { 96, 96, 96 };
        const int2 half = { size.x / 2, size.y / 2 };
        for (int i = 0; i < zox_tst_gpu_views_count; i++) {
            const int2 corner = { (i % 2) * half.x, (i / 2) * half.y };
            int drawn = 0, total = 0;
            for (int y = corner.y + half.y / 4; y < corner.y + half.y * 3 / 4; y++) {
                for (int x = corner.x + half.x / 4; x < corner.x + half.x * 3 / 4; x++) {
                    drawn += zox_tst_pixel_difference(zox_tst_ppm_pixel(pixels, size, x, y), gray) > 24;
                    total++;
                }
            }
            zox_log("[tst] view [%s]: [%.1f%%] of its middle is the models", zox_tst_gpu_views_names[i], 100.0f * drawn / total);
            zox_tst_check(drawn * 50 > total, "view [%s] didn't draw the gpu meshes", zox_tst_gpu_views_names[i]);
        }
        free(pixels);
    }
    for (int i = 0; i < zox_tst_gpu_models; i++) {
        gpu_mesher_dispose(&zox_tst_gpu_meshers[i]);
    }
    free(zox_tst_gpu_voxels);
}

// draws the gpu meshes in each 3D camera's pass, while the test runs (its GpuMeshViews entity)
void gpu_mesh_views_render_system(iter* it) {
    zox_sys_on_begin();
    if (!zox_tst_gpu_draw_program) {
        return;
    }
    for (int i = 0; i < zox_tst_gpu_models; i++) {
        gpu_mesher_draw(&zox_tst_gpu_meshers[i], zox_tst_gpu_draw_program, render_camera_matrix,
            zox_tst_gpu_model_position(i), zox_tst_gpu_scale, zox_tst_gpu_colors, zox_tst_gpu_colors_count);
    }
    zox_sys_on_end();
} zoxd_system(gpu_mesh_views_render_system);

void zox_tsts_models_gpu(ecs* world) {
    zoxd_tag(GpuMeshViews);
    zox_tst_add("gpu_mesh_views", zox_tst_gpu_mesh_views_ready, zox_tst_gpu_mesh_views_check, 60);
    zox_tst_set_setup("gpu_mesh_views", NULL, 1);
    zox_render3_system(2,
        gpu_mesh_views_render_system,
        [none] models.GpuMeshViews
    );
}
