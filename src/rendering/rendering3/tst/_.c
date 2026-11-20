// Rendering tests: each waits for a drawable candidate, then checks its render system drew over a few frames
//  (zox_drawn_* counters, which only go up)
//  - a condition, not a time: ready as soon as the counter goes up after a candidate is seen, or after the window
//    (vox instances were drawn some frames after their renderer, a fixed 3 frames failed 1 run in 3)
//  - on failure, diagnoses the candidate with the same skips its render system uses
//  - bash bsh/tst.sh skeleton_render | vox_instance_render | terrain_render

const int zox_tst_draws_settle = 3;    // frames a candidate must exist before the baseline
const int zox_tst_draws_window = 120;  // frames, at most, to see it drawn (~2s)

typedef struct {
    entity candidate;      // set by a probe system
    int frames;            // since the candidate was first seen
    uint baseline;
    byte has_baseline;
} zox_tst_draws;

// call from a ready function (once a frame), returns 1 once the draw window has passed
byte zox_tst_draws_ready(ecs* world, zox_tst_draws* draws, uint drawn, const char* waiting) {
    zox_tst_waiting = waiting;
    if (!zox_valid(draws->candidate)) {
        draws->candidate = 0;
        draws->frames = 0;
        draws->has_baseline = 0;
        return 0;
    }
    draws->frames++;
    if (!draws->has_baseline && draws->frames >= zox_tst_draws_settle) {
        draws->baseline = drawn;
        draws->has_baseline = 1;
    }
    return draws->has_baseline && (drawn > draws->baseline || draws->frames >= zox_tst_draws_settle + zox_tst_draws_window);
}

// diagnoses a candidate's camera filter, see camera_filtering_check
void zox_tst_check_camera_filter(ecs* world, entity e) {
    entity camera = zox_get_link(world, e, CameraRenderer);
    zox_tst_check(!camera || camera == renderer_camera,
        "[%s] is filtered to camera [%s], rendering camera is [%s]",
        zox_getn(e), zox_getn(camera), zox_getn(renderer_camera));
}

#include "skeleton.c"
#include "vox_instance.c"
#include "terrain.c"

void zox_tsts_rendering3(ecs* world) {
    zox_tsts_skeleton_render(world);
    zox_tsts_vox_instance_render(world);
    zox_tsts_terrain_render(world);
}
