// Rendering tests: each waits for a drawable candidate to settle, then checks its
// render system drew in the last second (zox_drawn_* counters).
//  - on failure, diagnoses the candidate with the same skips its render system uses
//  - bash bsh/tst.sh skeleton_render | vox_instance_render | terrain_render

const double zox_tst_draws_settle = 4.0;   // seconds a candidate must exist before checking
const double zox_tst_draws_window = 1.0;   // seconds of drawing that are checked

typedef struct {
    entity candidate;      // set by a probe system
    double first_seen;
    uint baseline;
    byte has_baseline;
} zox_tst_draws;

// call from a ready function, returns 1 once the draw window has passed
byte zox_tst_draws_ready(ecs* world, zox_tst_draws* draws, uint drawn, const char* waiting) {
    zox_tst_waiting = waiting;
    if (!zox_valid(draws->candidate)) {
        draws->candidate = 0;
        draws->first_seen = 0;
        draws->has_baseline = 0;
        return 0;
    }
    if (!draws->first_seen) {
        draws->first_seen = zox_current_time;
    }
    const double age = zox_current_time - draws->first_seen;
    if (!draws->has_baseline && age >= zox_tst_draws_settle - zox_tst_draws_window) {
        draws->baseline = drawn;
        draws->has_baseline = 1;
    }
    return draws->has_baseline && age >= zox_tst_draws_settle;
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
