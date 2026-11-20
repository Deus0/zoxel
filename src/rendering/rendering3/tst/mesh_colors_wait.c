// tst: mesh_colors_wait, a mesh's new colors wait for its new geometry (mesh_colors_gpu_system skips MeshDirty)
//  - a textured mesh (a quad) uploads, then remeshes to nothing while its lights build (MeshDirty, BuildMeshColors)
//    with its new colors ready (MeshColorsDirty): the gpu keeps drawing the old quad until the lights are done
//  - checks: its colors aren't uploaded alone meanwhile (MeshColorsDirty stays), the old quad still drawn
//  - the bug: none of its 4 vertices had a color on the gpu, nvidia's driver read them from null and crashed, about
//    1 game start in 3 (windows, tst ground_removed, 2026-11-21)
//  - run: bash bsh/tst.sh mesh_colors_wait
int zox_tst_mcw_stage = 0;
int zox_tst_mcw_frames = 0;
entity zox_tst_mcw_mesh = 0;
byte zox_tst_mcw_lights = 0;

static inline void zox_tst_mcw_spawn(ecs* world) {
    const entity e = zox_new();
    zox_setv(e, MeshGPULink, spawn_gpu_mesh_buffers());
    zox_setv(e, UvsGPULink, zox_gpu_create_buffer());
    zox_setv(e, ColorsGPULink, zox_gpu_create_buffer());
    MeshIndicies indicies = { 0 };
    MeshVertices vertices = { 0 };
    MeshUVs uvs = { 0 };
    MeshColorRGBs colors = { 0 };
    initialize_MeshIndicies(&indicies, 6);
    initialize_MeshVertices(&vertices, 4);
    initialize_MeshUVs(&uvs, 4);
    initialize_MeshColorRGBs(&colors, 4);
    const int quad[6] = { 0, 1, 2, 2, 3, 0 };
    memcpy(indicies.value, quad, sizeof(quad));
    for (int i = 0; i < 4; i++) {
        vertices.value[i] = (float3) { (float) (i % 2), (float) (i / 2), 0 };
        uvs.value[i] = (float2) { (float) (i % 2), (float) (i / 2) };
        colors.value[i] = (color_rgb) { 255, 255, 255 };
    }
    zox_set_ptr(e, MeshIndicies, indicies);
    zox_set_ptr(e, MeshVertices, vertices);
    zox_set_ptr(e, MeshUVs, uvs);
    zox_set_ptr(e, MeshColorRGBs, colors);
    zox_setv(e, MeshRenderCount, 0);
    zox_add(e, TexturedMesh3);
    zox_add(e, MeshDirty);
    zox_tst_mcw_mesh = e;
}

byte zox_tst_mesh_colors_wait_ready(ecs* world) {
    zox_tst_mcw_frames++;
    zox_tst_progress(zox_tst_mcw_frames);
    const entity e = zox_tst_mcw_mesh;
    if (zox_tst_mcw_stage == 0) {
        zox_tst_waiting = "the quad to upload";
        // the upload waits for lights only with smooth lighting on (textured_mesh_update_system)
        zox_tst_mcw_lights = zox_smooth_lighting && !zox_no_lights;
        zox_tst_mcw_spawn(world);
        zox_tst_mcw_stage = 1;
        return 0;
    }
    if (zox_tst_mcw_stage == 1) {
        if (!zox_has(e, MeshBuilt) || zox_getv(e, MeshRenderCount) != 6) {
            return 0;
        }
        // remeshed to nothing (the ground removed), its new colors ready, its lights still building
        MeshColorRGBs none = { 0 };
        zox_set_ptr(e, MeshColorRGBs, none);
        zox_add(e, MeshDirty);
        zox_add(e, BuildMeshColors);
        zox_add(e, MeshColorsDirty);
        zox_tst_mcw_stage = 2;
        zox_tst_mcw_frames = 0;
        return 0;
    }
    // the uploads run every frame: a few frames, while nothing should change
    zox_tst_waiting = "the uploads to run, with the lights still building";
    return zox_tst_mcw_frames > 3;
}

void zox_tst_mesh_colors_wait_check(ecs* world) {
    const entity e = zox_tst_mcw_mesh;
    zox_tst_check(zox_tst_mcw_lights, "smooth lighting is off, uploads don't wait for lights");
    zox_tst_check(zox_has(e, MeshColorsDirty),
        "its new colors were uploaded alone, under the old geometry it still draws");
    zox_tst_check(zox_getv(e, MeshRenderCount) == 6, "it draws [%u] indices, not the old quad's 6",
        zox_getv(e, MeshRenderCount));
}

void zox_tsts_mesh_colors_wait(ecs* world) {
    zox_tst_add("mesh_colors_wait", zox_tst_mesh_colors_wait_ready, zox_tst_mesh_colors_wait_check, 20);
    zox_tst_set_setup("mesh_colors_wait", NULL, 1);
}
