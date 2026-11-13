// Ui elements pop in and out: their LocalScale1 tweened (animations' lerp_float_system), the 2d hierarchy scales
//  their children with them, i.e. a window opening, its button appearing, it closing
const float ui_pop_in_time = 0.22f;
const float ui_pop_out_time = 0.16f;

// grows from nothing to its size, overshooting a little
void ui_pop_in(ecs* world, entity e) {
    if (!zox_valid(e)) {
        return;
    }
    zox_setv(e, LocalScale1, 0);
    const entity tween = zox_lerp_float_ins(e, LocalScale1, 0, 1, ui_pop_in_time);
    zox_setv(tween, AnimationEase, zox_ease_out_back);
}

// shrinks to nothing, then it's deleted (its children with it)
void ui_pop_out(ecs* world, entity e) {
    if (!zox_valid(e)) {
        return;
    }
    const float from = zox_has(e, LocalScale1) ? zox_getv(e, LocalScale1) : 1;
    const entity tween = zox_lerp_float_ins(e, LocalScale1, from, 0, ui_pop_out_time);
    zox_setv(tween, AnimationEase, zox_ease_in_back);
    zox_add(tween, AnimationDeletesTarget);
}
