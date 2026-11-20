// tst: particles_render, an emitter's particles are drawn where the camera looks (they went undrawn on vulkan)
//  - in a game: a screenshot of the view (tst/particles_render_before.ppm), then an emitter of bright magenta
//    particles just in front of the camera, a few frames, another (tst/particles_render.ppm)
//  - checks: magenta pixels appear in the middle of the screen (there were none before), and the particles live
//  - run: bash bsh/tst.sh particles_render [--vulkan]; compare backends: bash bsh/compare_backends.sh
extern char zox_screenshot_request[256];
#define zox_tst_particles_render_spawn_frame 3
#define zox_tst_particles_render_shot_frame 20
int zox_tst_particles_render_frame = 0;
entity zox_tst_particles_render_emitter = 0;

// pixels near magenta in the middle half of the screen
int zox_tst_particles_render_magenta(const char* file) {
    int2 size;
    byte* pixels = zox_tst_load_ppm(zox_tst_path(file), &size);
    if (!pixels) {
        return -1;
    }
    int count = 0;
    for (int y = size.y / 4; y < size.y * 3 / 4; y++) {
        for (int x = size.x / 4; x < size.x * 3 / 4; x++) {
            const byte* p = zox_tst_ppm_pixel(pixels, size, x, y);
            count += p[0] > 150 && p[2] > 150 && p[1] < 100;
        }
    }
    free(pixels);
    return count;
}

byte zox_tst_particles_render_ready(ecs* world) {
    if (!zox_tst_particles_render_frame && !zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    zox_tst_particles_render_frame++;
    zox_tst_waiting = "the particles to render";
    if (zox_tst_particles_render_frame == 1) {
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("particles_render_before.ppm"));
    } else if (zox_tst_particles_render_frame == zox_tst_particles_render_spawn_frame) {
        // just in front of the camera, along its ray through the middle of the screen
        const entity camera = zox_get_link(world, dbg_player, CameraLink);
        const byte has_ray = zox_valid(camera) && zox_has(camera, RaycastOrigin) && zox_has(camera, RaycastNormal);
        const float3 origin = has_ray ? zox_getv(camera, RaycastOrigin) : zox_getv(camera, Position3D);
        const float3 normal = has_ray ? zox_getv(camera, RaycastNormal) : (float3) { 0, 0, 1 };
        const float3 center = float3_add(origin, float3_scale(normal, 1.5f));
        zox_tst_particles_render_emitter = spawn_particle3D_emitter_at(world, center, 40, (float3) { 0.3f, 0.3f, 0.3f },
            (color) { 255, 0, 255, 255 });
    } else if (zox_tst_particles_render_frame == zox_tst_particles_render_shot_frame) {
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("particles_render.ppm"));
    }
    return zox_tst_particles_render_frame > zox_tst_particles_render_shot_frame && !zox_screenshot_request[0];
}

void zox_tst_particles_render_check(ecs* world) {
    const int before = zox_tst_particles_render_magenta("particles_render_before.ppm");
    const int after = zox_tst_particles_render_magenta("particles_render.ppm");
    zox_log("[tst] [metric] particles_render magenta pixels before [%i] after [%i] particles [%i]", before, after,
        zox_count_types(Particle3D));
    zox_tst_check(before >= 0 && after >= 0, "a screenshot wasn't saved");
    zox_tst_check(zox_count_types(Particle3D) > 0, "no particles live");
    zox_tst_check(after > before + 50, "the particles weren't drawn: [%i] magenta pixels, [%i] before", after, before);
    zox_delete(zox_tst_particles_render_emitter);
}

void zox_tsts_particles3(ecs* world) {
    zox_tst_add("particles_render", zox_tst_particles_render_ready, zox_tst_particles_render_check, 120);
}
