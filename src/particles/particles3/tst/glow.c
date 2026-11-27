// tst: particle_glow, a particle's Brightness past 1 draws it past white (doc/plans/hdr.md): two emitters of the same
//  warm colour side by side, the left's particles at brightness 1, the right's at 3 (an emitter's Brightness, given to
//  what it spawns, sys/emit.c)
//  - the menu's camera in a black void (zox_tst_lids_make_void), up in an empty sky, the emitters 1.5 in front of it
//  - views: clamp (tst/particle_glow/clamp.ppm), then bloom on (bloom.ppm), to look at
//  - checks: the right's lit pixels brighter than the left's, by over 1.3x (128 64 20 at 3 clips to 255 192 60)
//  - run: bash bsh/tst.sh particle_glow [--vulkan]
byte zox_tst_pglow_stage = 0;
int zox_tst_pglow_frames = 0;
entity zox_tst_pglow_emitters[2] = { 0 };
const char* zox_tst_pglow_files[2] = { "clamp.ppm", "bloom.ppm" };

byte zox_tst_particle_glow_ready(ecs* world) {
    const entity camera = main_cameras[0];
    if (!zox_valid(camera)) {
        zox_tst_waiting = "the menu's camera";
        return 0;
    }
    if (!zox_tst_pglow_emitters[0]) {
        zox_tst_lids_make_void(world, camera);
        zox_setv(camera, Color, ((color) { 0, 0, 0, 255 }));
        float3 eye = zox_getv(camera, Position3D);
        eye.y += 500;
        zox_setv(camera, Position3D, eye);
        const float4 rotation = zox_getv(camera, Rotation3D);
        const float3 ahead = move_along_direction(eye, rotation, -1.5f);
        const float3 right = float4_rotate_float3(rotation, (float3) { 0.35f, 0, 0 });
        for (byte i = 0; i < 2; i++) {
            const float3 at = float3_add(ahead, float3_scale(right, i ? 1 : -1));
            zox_tst_pglow_emitters[i] = spawn_particle3D_emitter_at(world, at, 2, (float3) { 0.15f, 0.15f, 0.15f },
                (color) { 128, 64, 20, 255 });
            zox_setv(zox_tst_pglow_emitters[i], Brightness, i ? 3 : 1);
        }
        nohud = 1;
        return 0;
    }
    zox_tst_waiting = "the particles' views";
    zox_tst_pglow_frames++;
    zox_tst_progress(zox_tst_pglow_stage * 1000 + zox_tst_pglow_frames);
    if (zox_screenshot_request[0]) {
        return 0;
    }
    if (zox_tst_pglow_stage >= 2) {
        return 1;
    }
    if (zox_tst_pglow_frames == 1) {
        zox_set_bloom(world, zox_tst_pglow_stage);
        return 0;
    }
    if (zox_tst_pglow_frames < 40) {
        return 0;
    }
    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(zox_tst_pglow_files[zox_tst_pglow_stage]));
    zox_tst_pglow_stage++;
    zox_tst_pglow_frames = 0;
    return 0;
}

// the mean brightness (0 to 765) of the lit pixels (over 150, the void has a little noise) in the image's left or right half, and how many
static inline int zox_tst_pglow_side(const byte* pixels, int2 size, byte right, int* lit) {
    long sum = 0;
    *lit = 0;
    for (int x = right ? size.x / 2 : 0; x < (right ? size.x : size.x / 2); x++) {
        for (int y = 0; y < size.y; y++) {
            const byte* p = zox_tst_ppm_pixel(pixels, size, x, y);
            const int v = p[0] + p[1] + p[2];
            if (v > 150) {    // over the void's noise (~54)
                sum += v;
                (*lit)++;
            }
        }
    }
    return *lit ? (int) (sum / *lit) : 0;
}

void zox_tst_particle_glow_check(ecs* world) {
    (void) world;
    int2 size;
    byte* image = zox_tst_load_ppm(zox_tst_path(zox_tst_pglow_files[0]), &size);
    zox_tst_check(image, "[%s] wasn't saved", zox_tst_pglow_files[0]);
    if (!image) {
        return;
    }
    int left_lit, right_lit;
    const int left = zox_tst_pglow_side(image, size, 0, &left_lit);
    const int right = zox_tst_pglow_side(image, size, 1, &right_lit);
    free(image);
    zox_log("[tst] particle_glow: brightness 1 [%i] over [%i] pixels, brightness 3 [%i] over [%i] pixels", left, left_lit,
        right, right_lit);
    zox_tst_check(left_lit > 20 && right_lit > 20, "the particles weren't drawn: [%i] and [%i] lit pixels", left_lit, right_lit);
    // 1.5 to 1.6x measured (random particles, alpha blended edges): over 1.3x, the same brightness ~1x
    zox_tst_check(right * 10 > left * 13, "brightness 3's particles [%i] aren't 1.3x brighter than 1's [%i]", right, left);
    for (byte i = 0; i < 2; i++) {
        zox_delete(zox_tst_pglow_emitters[i]);
    }
}
