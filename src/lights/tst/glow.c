// tst: glow_view, a glowing block (BlockGlow: glow stones) draws brighter than white (doc/plans/hdr.md): its faces'
//  colour alpha on the cpu (lights3 build_smooth), the draw's glows by voxel on the gpu (lights3 gpu_mesh)
//  - and its light is blocks' (its own tree, not the sky's), in its chunk's mesh colours' g (the sky's r)
//  - a chunk3 parent's top middle chunk (chunk3_parent.c): a glow stone alone in the sky's air
//  - a camera of its own straight to the screen, a voxel and a half in front of it, the game's view hidden
//  - its warm colour (255 226 150): unlit by glow its blue is at most its texture's (~142); glowing (BlockGlow 85,
//    2x) its red and green clip and its blue past ~200: past white, its stone's cracks still showing
//  - run: bash bsh/tst.sh glow_view
extern char zox_screenshot_request[256];
byte zox_tst_gview_stage = 0;
int zox_tst_gview_frames = 0;

byte zox_tst_gview_block = 0;        // blocks' light over the glow stone
byte zox_tst_gview_mesh_block = 0;   // the most blocks' light in its chunk's mesh colours
byte zox_tst_gview_gpu = 0;          // gpu meshed: no cpu mesh colours to read

byte zox_tst_glow_view_ready(ecs* world) {
    if (!zox_tst_chunk3_parent_ready(world)) {
        return 0;
    }
    const int size = octree_size(terrain_depth);
    const byte3 local = { 5, 7, 5 };
    if (zox_tst_gview_stage == 0) {
        const byte glow = zox_tst_chunks_block(world, zox_block_material_solid, 230);
        zox_tst_check(glow, "the realm has no glow stone (solid, light 230)");
        VoxelNodeQueue* queue = zox_getm(zox_tst_chunk_at(world, (int3) { 0, 1, 0 }), VoxelNodeQueue);
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = glow, .position = local });
        zox_tst_gview_stage = 1;
        return 0;
    }
    if (zox_tst_gview_stage == 1) {
        // its middle, the chunk (0, 1, 0)'s voxel at local, half a unit a voxel (the parent's 0.5)
        const float3 target = voxel_to_real_position((int3) { local.x, size + local.y, local.z }, 0.5f, 0.5f);
        const float3 eye = float3_add(target, (float3) { 0, 0.2f, 0.75f });
        const entity camera = spawn_viewport_camera(world, main_app, eye, camera_look_at(eye, target), 60,
            viewport_grid_cell(0, 1, 1));
        zox_setv(camera, Color, ((color) { 20, 20, 30, 255 }));
        nohud = 1;
        const entity game_view = zox_get_link(world, main_cameras[0], RenderTextureLink);
        if (zox_valid(game_view)) {
            zox_setv(game_view, RenderDisabled, 1);
        }
        zox_tst_gview_stage = 2;
        return 0;
    }
    if (zox_tst_gview_stage == 2) {
        zox_tst_waiting = "the view drawn";
        if (++zox_tst_gview_frames < 10) {
            return 0;
        }
        // its light in blocks' tree (not the sky's), and in its chunk's mesh colours' g (lights3/sys/build_smooth.c)
        const entity chunk = zox_tst_chunk_at(world, (int3) { 0, 1, 0 });
        const LightOctree* block = chunk_lights(world, chunk, zox_light_block);
        zox_tst_gview_block = block ? getv_LightOctree(block, zox_getv(chunk, NodeDepth), (byte3) { local.x, local.y + 1, local.z }) : 0;
        entity meshes[zox_chunk_meshes_max];
        const uint count = get_chunk_meshes(world, chunk, meshes);
        for (uint i = 0; i < count; i++) {
            if (zox_has(meshes[i], GpuMesh)) {
                // its lights in its gpu buffer (ColorsGPULink): each vertex the sky's corners, then blocks' (a byte each)
                zox_tst_gview_gpu = 1;
                const uint vertices = zox_has(meshes[i], MeshRenderCount) ? zox_getv(meshes[i], MeshRenderCount) / 6 * 4 : 0;
                const guint buffer = zox_has(meshes[i], ColorsGPULink) ? zox_getv(meshes[i], ColorsGPULink) : 0;
                uint* lights = vertices && buffer ? malloc(vertices * 2 * sizeof(uint)) : NULL;
                if (lights) {
                    zox_gpu_storage_read(buffer, 0, vertices * 2 * sizeof(uint), lights);
                    for (uint v = 0; v < vertices; v++) {
                        for (byte k = 0; k < 4; k++) {
                            const byte b = (lights[v * 2 + 1] >> (k * 8)) & 255;
                            if (b > zox_tst_gview_mesh_block) {
                                zox_tst_gview_mesh_block = b;
                            }
                        }
                    }
                    free(lights);
                }
                continue;
            }
            const MeshColors* colors = zox_has(meshes[i], MeshColors) ? zox_get(meshes[i], MeshColors) : NULL;
            for (int c = 0; colors && c < colors->length; c++) {
                if (colors->value[c].g > zox_tst_gview_mesh_block) {
                    zox_tst_gview_mesh_block = colors->value[c].g;
                }
            }
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("glow_view.ppm"));
        zox_tst_gview_stage = 3;
        return 0;
    }
    zox_tst_waiting = "the screenshot";
    return !zox_screenshot_request[0];
}

void zox_tst_glow_view_check(ecs* world) {
    (void) world;
    int2 size;
    byte* image = zox_tst_load_ppm(zox_tst_path("glow_view.ppm"), &size);
    zox_tst_check(image, "glow_view.ppm wasn't saved");
    if (!image) {
        return;
    }
    int r = 0, g = 0, b = 0, count = 0;
    for (int i = -4; i <= 4; i++) for (int j = -4; j <= 4; j++) {
        const byte* p = zox_tst_ppm_pixel(image, size, size.x / 2 + i, size.y / 2 + j);
        r += p[0];
        g += p[1];
        b += p[2];
        count++;
    }
    free(image);
    r /= count;
    g /= count;
    b /= count;
    zox_log("[tst] glow_view: the glow stone's middle [%i %i %i], blocks' light over it [%i], in its mesh colours [%i]", r, g, b,
        zox_tst_gview_block, zox_tst_gview_mesh_block);
    zox_tst_check(zox_tst_gview_block > darklight, "blocks' light over the glow stone is [%i]: not in its own tree", zox_tst_gview_block);
    zox_tst_check(zox_tst_gview_mesh_block > darklight, "its chunk's mesh (colours' g, or gpu vertex lights) carry no blocks' light [%i] gpu [%i]",
        zox_tst_gview_mesh_block, zox_tst_gview_gpu);
    zox_tst_check(r > 200 && g > 180, "the glow stone isn't in view, or dark [%i %i %i]", r, g, b);
    zox_tst_check(b > 180, "the glow stone's blue [%i]: not past white (its texture's is ~142 unglowing)", b);
}
