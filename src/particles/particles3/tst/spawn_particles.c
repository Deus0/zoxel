// tst: spawn_particles, the spawn arrow's particles (controllers3 character_spawner.c: 4 emitters up the arrow, a colour
//  each) as a gif and a screenshot, to see their size, their see through and their fade (sys/fade.c)
//  - a new game's spawn, its camera third person (behind and above the head, looking down at the arrow)
//  - tst/spawn_particles.gif (3s at zox_tst_gif_fps, 15 by default) and tst/spawn_particles.ppm
//  - checks: the gif has frames, the particles live and fade (their average Alpha under 1)
//  - run: bash bsh/tst.sh spawn_particles [--vulkan]
#define zox_tst_spawn_particles_seconds 3.0
int zox_tst_spawn_particles_stage = 0;
int zox_tst_spawn_particles_frames = 0;
double zox_tst_spawn_particles_time = 0;
int zox_tst_spawn_particles_gif_frames = 0;
float zox_tst_spawn_particles_alpha = 1;

byte zox_tst_spawn_particles_ready(ecs* world) {
    zox_tst_progress(zox_tst_spawn_particles_stage * 1000 + zox_tst_spawn_particles_frames);
    if (zox_tst_spawn_particles_stage == 0) {
        zox_tst_waiting = "the player to spawn";
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        const entity camera = zox_get_link(world, dbg_player, CameraLink);
        if (!zox_valid(camera)) {
            return 0;
        }
        set_camera_mode(world, camera, zox_camera_state_third_person);
        set_camera_transform(world, camera, 0, zox_camera_state_third_person);    // attached: no character needed
        zox_tst_spawn_particles_stage = 1;
        return 0;
    }
    zox_tst_spawn_particles_frames++;
    if (zox_tst_spawn_particles_stage == 1) {
        // the camera settled, the emitters streaming a while
        zox_tst_waiting = "the camera to settle";
        if (zox_tst_spawn_particles_frames < 30) {
            return 0;
        }
        const char* env = getenv("zox_tst_gif_fps");
        zox_gif_record(zox_tst_path("spawn_particles.gif"), env && env[0] ? atof(env) : 15);
        zox_tst_spawn_particles_time = zox_current_time;
        zox_tst_spawn_particles_stage = 2;
        return 0;
    }
    if (zox_tst_spawn_particles_stage == 2) {
        zox_tst_waiting = "the gif";
        if (zox_current_time - zox_tst_spawn_particles_time < zox_tst_spawn_particles_seconds) {
            return 0;
        }
        zox_tst_spawn_particles_gif_frames = zox_gif_stop();
        zox_tst_spawn_particles_alpha = zox_tst_particles_alpha(world);
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("spawn_particles.ppm"));
        zox_tst_spawn_particles_stage = 3;
        return 0;
    }
    zox_tst_waiting = "the screenshot";
    return !zox_screenshot_request[0];
}

void zox_tst_spawn_particles_check(ecs* world) {
    zox_gif_stop();
    zox_log("[tst] [metric] spawn_particles: [%i] gif frames, [%i] particles, their fade [%.2f]",
        zox_tst_spawn_particles_gif_frames, zox_count_types(Particle3D), zox_tst_spawn_particles_alpha);
    zox_tst_check(zox_tst_spawn_particles_gif_frames >= 10, "the gif has [%i] frames", zox_tst_spawn_particles_gif_frames);
    zox_tst_check(zox_count_types(Particle3D) > 0, "no particles live");
    zox_tst_check(zox_tst_spawn_particles_alpha < 0.95f, "the particles don't fade, their alpha [%.2f]",
        zox_tst_spawn_particles_alpha);
}
