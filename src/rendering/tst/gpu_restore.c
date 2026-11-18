// tst: gpu_restore, disposing every gpu resource and restoring them (an app minimized, android's lost context, or a
//  fullscreen alt tab: opengl_dispose_resources then opengl_restore_resources) gives the same picture back
//  - on the main menu (ui meshes, textures, shaders, materials, the render camera's buffers)
//  - counts each gpu link that holds something, takes a screenshot (two, to see how much it moves on its own)
//  - disposes: every gpu link is let go of (0), it stops rendering
//  - restores: as many gpu links hold something as before, no opengl errors, a screenshot like the one before (its
//    average colour: the sky came back magenta, the skybox didn't keep its menu colours)
//  - live gpu objects (buffers, textures, shaders, programs, fbos, rbos): none after disposing (a lost context
//    leaves anything kept dead, i.e. a global material), as many after restoring as before (no leaks)
//  - a material's uniform locations are looked up again (scrambled before, the same as at first after)
//  - run: bash bsh/tst.sh gpu_restore
extern char zox_screenshot_request[256];
#define zox_tst_gr_types 8
const char* zox_tst_gr_names[zox_tst_gr_types] = {
    "MeshGPULink", "UvsGPULink", "ColorsGPULink", "TextureGPULink", "ShaderGPULink", "MaterialGPULink",
    "FrameBufferLink", "RenderBufferLink" };
int zox_tst_gr_before[zox_tst_gr_types];
int zox_tst_gr_disposed[zox_tst_gr_types];
int zox_tst_gr_restored[zox_tst_gr_types];
int zox_tst_gr_stage = 0;
int zox_tst_gr_frames = 0;
byte zox_tst_gr_stopped = 0, zox_tst_gr_rendering = 0, zox_tst_gr_errors = 0;
#define zox_tst_gr_kinds 6
const char* zox_tst_gr_kind_names[zox_tst_gr_kinds] = { "buffers", "textures", "shaders", "programs", "fbos", "rbos" };
int zox_tst_gr_live_before[zox_tst_gr_kinds];
int zox_tst_gr_live_disposed[zox_tst_gr_kinds];
int zox_tst_gr_live_restored[zox_tst_gr_kinds];
MaterialBasic3D zox_tst_gr_locations;      // material_basic3D's, before
byte zox_tst_gr_relocated = 0;
float zox_tst_gr_moves = 0, zox_tst_gr_changed = 0;   // average colour changes (0 to 765): on its own, and by a restore

// how many of a gpu link's entities hold something
#define zox_tst_gr_count_links(T, holds)\
{\
    int count = 0;\
    iter it = zox_each(world, T);\
    while (zox_each_next(it)) {\
        for (int j = 0; j < it.count; j++) {\
            const T* link = zox_get(it.entities[j], T);\
            if (link && (holds)) {\
                count++;\
            }\
        }\
    }\
    counts[index++] = count;\
}

void zox_tst_gr_count(ecs* world, int* counts) {
    int index = 0;
    zox_tst_gr_count_links(MeshGPULink, link->value.x || link->value.y);
    zox_tst_gr_count_links(UvsGPULink, link->value);
    zox_tst_gr_count_links(ColorsGPULink, link->value);
    zox_tst_gr_count_links(TextureGPULink, link->value);
    zox_tst_gr_count_links(ShaderGPULink, link->value.x || link->value.y);
    zox_tst_gr_count_links(MaterialGPULink, link->value);
    zox_tst_gr_count_links(FrameBufferLink, link->value);
    zox_tst_gr_count_links(RenderBufferLink, link->value);
}

// live gpu objects of each kind (counted as made and let go of, src/_/logs)
void zox_tst_gr_live(int* counts) {
    const int live[zox_tst_gr_kinds] = { zox_gpu_buffers_live, zox_gpu_textures_live, zox_gpu_shaders_live,
        zox_gpu_programs_live, zox_gpu_fbos_live, zox_gpu_rbos_live };
    memcpy(counts, live, sizeof(live));
}

void zox_tst_gr_screenshot(const char* file) {
    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(file));
}

// how far apart two screenshots' average colours are (0 to 765), 765 if either is missing or they differ in size
//  - an average, so things moving (the menu's spinning cube) barely change it, a wrong sky, missing textures or a
//    black screen do
float zox_tst_gr_compare(const char* file_a, const char* file_b) {
    char path_a[256];
    snprintf(path_a, sizeof(path_a), "%s", zox_tst_path(file_a));
    int2 size_a, size_b;
    byte* a = zox_tst_load_ppm(path_a, &size_a);
    byte* b = zox_tst_load_ppm(zox_tst_path(file_b), &size_b);
    float difference = 765;
    if (a && b && int2_equals(size_a, size_b)) {
        const size_t length = (size_t) size_a.x * size_a.y * 3;
        double sum_a[3] = { 0 }, sum_b[3] = { 0 };
        for (size_t i = 0; i < length; i++) {
            sum_a[i % 3] += a[i];
            sum_b[i % 3] += b[i];
        }
        const double pixels = (double) size_a.x * size_a.y;
        difference = 0;
        for (int c = 0; c < 3; c++) {
            difference += fabs(sum_a[c] - sum_b[c]) / pixels;
        }
    }
    free(a);
    free(b);
    return difference;
}

byte zox_tst_gpu_restore_ready(ecs* world) {
    const entity canvas = zox_valid(dbg_player) ? zox_get_link(world, dbg_player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_gr_frames++;
    zox_tst_progress(zox_tst_gr_stage);
    if (zox_tst_gr_stage == 0) {
        // the main menu settled and rendering (a test's window may start frozen as minimized)
        zox_tst_waiting = "the main menu to settle";
        opengl_restore_resources(world);
        if (zox_tst_gr_frames < 60) {
            return 0;
        }
        zox_tst_gr_screenshot("gpu_restore_a.ppm");
        zox_tst_gr_stage = 1;
        zox_tst_gr_frames = 0;
        return 0;
    }
    if (zox_tst_gr_stage == 1) {
        zox_tst_waiting = "a second screenshot";
        if (zox_screenshot_request[0] || zox_tst_gr_frames < 5) {
            return 0;
        }
        zox_tst_gr_screenshot("gpu_restore_b.ppm");
        zox_tst_gr_stage = 2;
        zox_tst_gr_frames = 0;
        return 0;
    }
    if (zox_tst_gr_stage == 2) {
        zox_tst_waiting = "the screenshots";
        if (zox_screenshot_request[0]) {
            return 0;
        }
        zox_tst_gr_moves = zox_tst_gr_compare("gpu_restore_a.ppm", "gpu_restore_b.ppm");
        zox_tst_gr_count(world, zox_tst_gr_before);
        zox_tst_gr_live(zox_tst_gr_live_before);
        check_opengl_error("gpu_restore before");
        MaterialBasic3D* locations = zox_get_mut(material_basic3D, MaterialBasic3D);
        zox_tst_gr_locations = *locations;
        memset(locations, 0xee, sizeof(MaterialBasic3D));
        opengl_dispose_resources(world);
        zox_tst_gr_count(world, zox_tst_gr_disposed);
        zox_tst_gr_live(zox_tst_gr_live_disposed);
        zox_tst_gr_stopped = !rendering;
        zox_tst_gr_stage = 3;
        zox_tst_gr_frames = 0;
        return 0;
    }
    if (zox_tst_gr_stage == 3) {
        zox_tst_waiting = "a few frames disposed";
        if (zox_tst_gr_frames < 10) {
            return 0;
        }
        opengl_restore_resources(world);
        zox_tst_gr_rendering = rendering;
        zox_tst_gr_relocated = !memcmp(zox_get(material_basic3D, MaterialBasic3D), &zox_tst_gr_locations, sizeof(MaterialBasic3D));
        zox_tst_gr_stage = 4;
        zox_tst_gr_frames = 0;
        return 0;
    }
    if (zox_tst_gr_stage == 4) {
        // meshes and textures re-upload over the next frames (MeshDirty, TextureDirty)
        zox_tst_waiting = "the restore to upload";
        zox_tst_gr_errors |= check_opengl_error("gpu_restore after");
        if (zox_tst_gr_frames < 60) {
            return 0;
        }
        zox_tst_gr_count(world, zox_tst_gr_restored);
        zox_tst_gr_live(zox_tst_gr_live_restored);
        zox_tst_gr_screenshot("gpu_restore_c.ppm");
        zox_tst_gr_stage = 5;
        zox_tst_gr_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the screenshot after";
    if (zox_screenshot_request[0]) {
        return 0;
    }
    zox_tst_gr_changed = zox_tst_gr_compare("gpu_restore_b.ppm", "gpu_restore_c.ppm");
    return 1;
}

void zox_tst_gpu_restore_check(ecs* world) {
    (void) world;
    for (int i = 0; i < zox_tst_gr_types; i++) {
        zox_log("[tst] [metric] %s: [%i] before, [%i] disposed, [%i] restored", zox_tst_gr_names[i],
            zox_tst_gr_before[i], zox_tst_gr_disposed[i], zox_tst_gr_restored[i]);
        zox_tst_check(zox_tst_gr_disposed[i] == 0, "disposed, [%i] %s still hold something", zox_tst_gr_disposed[i], zox_tst_gr_names[i]);
        zox_tst_check(zox_tst_gr_restored[i] >= zox_tst_gr_before[i], "restored, [%i] %s hold something, [%i] did before",
            zox_tst_gr_restored[i], zox_tst_gr_names[i], zox_tst_gr_before[i]);
    }
    for (int i = 0; i < zox_tst_gr_kinds; i++) {
        zox_log("[tst] [metric] live %s: [%i] before, [%i] disposed, [%i] restored", zox_tst_gr_kind_names[i],
            zox_tst_gr_live_before[i], zox_tst_gr_live_disposed[i], zox_tst_gr_live_restored[i]);
        zox_tst_check(zox_tst_gr_live_disposed[i] == 0, "disposed, [%i] %s still live (kept outside the gpu links?)",
            zox_tst_gr_live_disposed[i], zox_tst_gr_kind_names[i]);
        zox_tst_check(zox_tst_gr_live_restored[i] <= zox_tst_gr_live_before[i], "restored, [%i] %s live, [%i] before (leaked)",
            zox_tst_gr_live_restored[i], zox_tst_gr_kind_names[i], zox_tst_gr_live_before[i]);
    }
    zox_tst_check(zox_tst_gr_before[0] > 0 && zox_tst_gr_before[3] > 0, "nothing on the gpu to test (no meshes or textures)");
    zox_tst_check(zox_tst_gr_stopped, "disposed, it kept rendering");
    zox_tst_check(zox_tst_gr_rendering, "restored, it isn't rendering");
    zox_tst_check(!zox_tst_gr_errors, "opengl errors after restoring");
    zox_tst_check(zox_tst_gr_relocated, "restored, material_basic3D's uniform locations weren't looked up again");
    zox_log("[tst] [metric] screenshots' average colour: [%.1f] changed on its own, [%.1f] after a restore (tst/gpu_restore_b.ppm, c)",
        zox_tst_gr_moves, zox_tst_gr_changed);
    zox_tst_check(zox_tst_gr_changed <= zox_tst_gr_moves + 12, "restored, the picture's average colour changed [%.1f] (on its own [%.1f])",
        zox_tst_gr_changed, zox_tst_gr_moves);
}
