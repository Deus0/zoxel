entity spawn_prefab_overlay(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("screen_overlay");
    zox_add(e, CanvasOverlay);
    zox_setv(e, FrameCorner, 0);
    zox_setv(e, AnimationState, 0);
    zox_setv(e, AnimationStart, 0);
    zox_setv(e, AnimationLength, 1.6f);
    zox_setv(e, AnimationDelay, 0.4f);
    // animation
    zox_setv(e, AnimationIndex, 0);
    zox_add(e, AnimationSequence);
    zox_add(e, AnimationTimes);
    zox_add(e, AnimationTargets);
    return e;
}
