// tst: walk_views, the walk as a gif from 4 angles, to look at its gait (bones/sys/arm_control.c, arm_swing.c)
//  - bash bsh/walk.sh (your settings), then look at tst/walk_views/walk.gif, or walk.png (its frames in a grid)
//  - the body_views setup (a character of the player's kind in the void), walking ahead (+z) at the player's walk
//      speed (zox_tst_walk_views_speed), held at its height (no ground), the 4 cameras following it: the right side, ahead, ahead
//      and right above, behind
//  - measures over its steps, in the body's space: the lower foot's height (steady: the stance foot stays on the
//      ground as the body bobs over it), the knees' most bend (the swinging leg lifts its foot), the head's bob, the
//      steps a second
//  - then crouched (CrouchState), walking slower: walk_views/crouch.gif; the feet on the same ground, the head lower
#define zox_tst_walk_views_settle 0.6       // seconds walking before the gif
#define zox_tst_walk_views_seconds 2.5      // the gif
#define zox_tst_walk_views_speed 1.4f       // metres a second, the player's walk (controllers3 max_velocity3D, a module above)
int zox_tst_walk_views_stage = 0;
double zox_tst_walk_views_time = 0;
#define zox_tst_walk_views_crouch_speed 0.6f    // crouched (controllers3 crouch_speed of the walk)
typedef struct {
    int frames;
    float foot_low, foot_high;      // the lower foot's height
    float head_low, head_high;
    float knee;                     // the most bend, degrees
    float thigh;                    // the most swing, degrees
    int steps;                      // the lower foot changing sides
    int lower;
} zox_tst_walk_measures;
#define zox_tst_walk_measures_start { 0, 99, -99, 99, -99, 0, 0, 0, -1 }
zox_tst_walk_measures zox_tst_walk_views_walk = zox_tst_walk_measures_start;
zox_tst_walk_measures zox_tst_walk_views_crouch = zox_tst_walk_measures_start;

static inline float zox_tst_walk_views_angle(float4 q) {
    return 2 * acosf(fminf(1, fabsf(q.w))) * radians_to_degrees;
}

static inline void zox_tst_walk_views_cameras(ecs* world, entity character) {
    const float3 target = float3_add(zox_getv(character, Position3D), (float3) { 0, -0.05f, 0 });
    const float distance = float3_length(zox_getv(character, Bounds3D)) / sinf(25 * degreesToRadians) * 0.85f;
    const float3 directions[zox_tst_body_views_count] = {
        { -1, 0.1f, 0 },        // the right side
        { 0, 0.15f, 1 },        // ahead
        { -0.7f, 0.5f, 0.7f },  // ahead, right, above
        { 0, 0.25f, -1 } };     // behind
    for (int i = 0; i < zox_tst_body_views_count; i++) {
        const float3 position = float3_add(target, float3_scale(float3_normalize(directions[i]), distance));
        zox_setv(zox_tst_body_views_cameras[i], Position3D, position);
        zox_setv(zox_tst_body_views_cameras[i], Rotation3D, camera_look_at(position, target));
    }
}

static inline void zox_tst_walk_views_measure(ecs* world, entity skeleton, zox_tst_walk_measures* m) {
    ik_leg legs[2];
    if (zox_tst_ik_poses_legs(world, skeleton, legs) != 2) {
        return;
    }
    // the skeleton's place the bones were put from (the body's after physics; reset after, it fell a little each frame)
    const float body = zox_getv(skeleton, Position3D).y;
    float feet[2];
    for (int i = 0; i < 2; i++) {
        feet[i] = zox_getv(legs[i].foot, Position3D).y - body;
        m->knee = fmaxf(m->knee, zox_tst_walk_views_angle(zox_getv(legs[i].calf, LocalRotation3D)));
        m->thigh = fmaxf(m->thigh, zox_tst_walk_views_angle(zox_getv(legs[i].thigh, LocalRotation3D)));
    }
    const int lower = feet[0] < feet[1] ? 0 : 1;
    if (m->lower >= 0 && lower != m->lower) {
        m->steps++;
    }
    m->lower = lower;
    m->foot_low = fminf(m->foot_low, feet[lower]);
    m->foot_high = fmaxf(m->foot_high, feet[lower]);
    const entity head = zox_get_link(world, skeleton, HeadBoneLink);
    if (zox_valid(head)) {
        const float y = zox_getv(head, Position3D).y - body;
        m->head_low = fminf(m->head_low, y);
        m->head_high = fmaxf(m->head_high, y);
    }
}

byte zox_tst_walk_views_ready(ecs* world) {
    if (zox_tst_body_views_stage < 3) {
        zox_tst_body_views_ready(world);
        return 0;
    }
    const entity character = zox_tst_body_views_character;
    if (!zox_valid(character)) {
        zox_tst_check(0, "the character is gone");
        return 1;
    }
    const entity skeleton = get_skeleton(world, character);
    // walking ahead at its height, the cameras with it
    float3 position = zox_getv(character, Position3D);
    position.x = zox_tst_body_views_spot.x;
    position.y = zox_tst_body_views_spot.y;
    zox_setv(character, Position3D, position);
    zox_setv(character, Rotation3D, quaternion_identity);
    const float speed = zox_tst_walk_views_stage >= 3 ? zox_tst_walk_views_crouch_speed : zox_tst_walk_views_speed;
    zox_setv(character, Velocity3D, ((float3) { 0, 0, zox_tst_walk_views_stage ? speed : 0 }));
    zox_setv(skeleton, CrouchState, zox_tst_walk_views_stage >= 3);
    zox_tst_walk_views_cameras(world, character);
    zox_tst_progress(zox_tst_walk_views_stage * 1000 + (zox_current_time - zox_tst_walk_views_time) * 10);
    if (zox_tst_walk_views_stage == 0) {
        const entity active = zox_get_link(world, skeleton, ActiveMesh);
        if (zox_getv(character, BodyDirty) != zox_generate_body_end || !zox_valid(active) ||
            zox_has(active, SkeletonMeshStale) || !is_skeleton_mesh_ready(world, active) ||
            zox_valid(zox_get_link(world, skeleton, PreparingMesh)))
        {
            zox_tst_waiting = "the body to settle";
            return 0;
        }
        zox_tst_walk_views_stage = 1;
        zox_tst_walk_views_time = zox_current_time;
        return 0;
    }
    const double t = zox_current_time - zox_tst_walk_views_time;
    if (zox_tst_walk_views_stage == 1) {
        zox_tst_waiting = "walking";
        if (t < zox_tst_walk_views_settle) {
            return 0;
        }
        const char* env = getenv("zox_tst_gif_fps");
        zox_gif_record(zox_tst_path("walk_views/walk.gif"), env && env[0] ? atof(env) : 20);
        zox_tst_walk_views_stage = 2;
        return 0;
    }
    if (zox_tst_walk_views_stage == 2) {
        zox_tst_waiting = "the walk's gif";
        zox_tst_walk_views_measure(world, skeleton, &zox_tst_walk_views_walk);
        if (t > zox_tst_walk_views_settle + zox_tst_walk_views_seconds) {
            zox_tst_walk_views_walk.frames = zox_gif_stop();
            zox_tst_walk_views_stage = 3;
            zox_tst_walk_views_time = zox_current_time;
        }
        return 0;
    }
    if (zox_tst_walk_views_stage == 3) {
        zox_tst_waiting = "crouching";
        if (t < zox_tst_walk_views_settle) {
            return 0;
        }
        const char* env = getenv("zox_tst_gif_fps");
        zox_gif_record(zox_tst_path("walk_views/crouch.gif"), env && env[0] ? atof(env) : 20);
        zox_tst_walk_views_stage = 4;
        return 0;
    }
    zox_tst_waiting = "the crouched walk's gif";
    zox_tst_walk_views_measure(world, skeleton, &zox_tst_walk_views_crouch);
    if (t > zox_tst_walk_views_settle + zox_tst_walk_views_seconds) {
        zox_tst_walk_views_crouch.frames = zox_gif_stop();
        return 1;
    }
    return 0;
}

void zox_tst_walk_views_check(ecs* world) {
    (void) world;
    zox_gif_stop();
    const zox_tst_walk_measures* w = &zox_tst_walk_views_walk;
    const zox_tst_walk_measures* c = &zox_tst_walk_views_crouch;
    const float foot = w->foot_high - w->foot_low;
    const float head = w->head_high - w->head_low;
    zox_log("[tst] walk_views: [%i] frames, [%.2f] steps a second, thighs swing [%.1f] degrees, knees bend [%.1f]",
        w->frames, w->steps / (float) zox_tst_walk_views_seconds, w->thigh, w->knee);
    zox_log("[tst] walk_views: the lower foot's height moves [%.3f], the head bobs [%.3f]", foot, head);
    zox_tst_check(w->frames >= 4, "[walk.gif] has [%i] frames", w->frames);
    // the old gait: 58 degree strides on straight legs, the feet 0.16 up and down through the ground, a head bob of its own
    zox_tst_check(foot < 0.03f, "the stance foot didn't stay on the ground, it moved [%f]", foot);
    zox_tst_check(w->knee > 25, "the knees didn't bend to lift the feet [%f]", w->knee);
    zox_tst_check(w->thigh > 10 && w->thigh < 40, "the stride is [%f] degrees", w->thigh);
    zox_tst_check(head > 0.005f && head < 0.06f, "the head bobbed [%f]", head);
    // crouched: the feet on the same ground (the lower one at the walk's lowest), the head well down
    const float ground = fmaxf(fabsf(c->foot_low - w->foot_low), fabsf(c->foot_high - w->foot_low));
    const float down = (w->head_low + w->head_high) * 0.5f - (c->head_low + c->head_high) * 0.5f;
    zox_log("[tst] walk_views: crouched [%i] frames, the lower foot off the walk's ground [%.3f], the head down [%.3f], knees bend [%.1f]",
        c->frames, ground, down, c->knee);
    zox_tst_check(c->frames >= 4, "[crouch.gif] has [%i] frames", c->frames);
    zox_tst_check(ground < 0.03f, "crouched, the feet left the ground [%f]", ground);
    zox_tst_check(down > 0.08f, "crouched, the head went down [%f]", down);
}

void zox_tsts_walk_views(ecs* world) {
    zox_tst_add("walk_views", zox_tst_walk_views_ready, zox_tst_walk_views_check, 120);
    zox_tst_set_setup("walk_views", NULL, 1);
}
