// Starts a user's arm swing (a punch, a place, eating): from start over time, its marks (sys/swing_mark.c) where it
//  hits, observers react to them; action is what's swung (a skill, an item), at a point it's at (a place's block),
//  zero none; set once a start (SwingStart is it)
void skeleton_swing_begin(ecs* world, entity user, entity action, double start, float time, byte kind, float3 at) {
    const entity skeleton = get_skeleton(world, user);
    if (!zox_valid(skeleton) || zox_getv(skeleton, SwingStart) == start) {
        return;
    }
    zox_setv(skeleton, SwingStart, start);
    zox_setv(skeleton, SwingSpeed, time);
    zox_setv(skeleton, SwingKind, kind);
    zox_setv(skeleton, SwingTarget, at);
    zox_setv(skeleton, SwingAction, action);
    zox_setv(skeleton, SwingMark, 0);   // its marks from its first (sys/swing_mark.c)
}
