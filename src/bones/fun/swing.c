// Starts a user's arm swing (a punch, a place, eating) at its action's warmup, over the warmup and cooldown: it lands
//  as the warmup ends, when the action happens; set once a warmup (SwingStart is its start); at, a point it's at
//  (a place's block), zero none
void skeleton_swing_begin(ecs* world, entity user, double start, float time, byte kind, float3 at) {
    const entity skeleton = get_skeleton(world, user);
    if (!zox_valid(skeleton) || zox_getv(skeleton, SwingStart) == start) {
        return;
    }
    zox_setv(skeleton, SwingStart, start);
    zox_setv(skeleton, SwingSpeed, time);
    zox_setv(skeleton, SwingKind, kind);
    zox_setv(skeleton, SwingTarget, at);
}
