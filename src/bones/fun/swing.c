// Starts a user's arm swing (a punch, a place) at its action's warmup, over the warmup and cooldown: the strike lands
//  as the warmup ends, when the action hits; set once a warmup (SwingStart is its start)
void skeleton_swing_begin(ecs* world, entity user, double start, float time, byte kind) {
    const entity skeleton = get_skeleton(world, user);
    if (!zox_valid(skeleton) || zox_getv(skeleton, SwingStart) == start) {
        return;
    }
    zox_setv(skeleton, SwingStart, start);
    zox_setv(skeleton, SwingSpeed, time);
    zox_setv(skeleton, SwingKind, kind);
}
