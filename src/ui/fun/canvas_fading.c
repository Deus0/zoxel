void clear_animation_sequence(ecs *world, entity e) {
    if (!zox_has(e, AnimationSequence)) {
        zox_loge("Entity [%s] has no AnimationSequence",
            zox_getn(e));
        return;
    }
    zox_muter(e, AnimationSequence, animationSequence);
    if (animationSequence->value) {
        clear_memory_component(AnimationSequence, animationSequence);
        zox_muter(e, AnimationTimes, animationTimes);
        clear_memory_component(AnimationTimes, animationTimes);
        zox_muter(e, AnimationTargets, animationTargets);
        clear_memory_component(AnimationTargets, animationTargets);
        zox_set(e, AnimationIndex, { 255 });
    }
}

// initial from game start
void trigger_canvas_overlay_fade_out(ecs *world, entity e, float delay, float length) {
    zox_set(e, AnimationState, { zox_animate_alpha });
    zox_set(e, AnimationStart, { zox_current_time });
    zox_set(e, AnimationDelay, { delay });
    zox_set(e, AnimationLength, {  length });
    zox_set(e, AnimateSourceFloat, { 1.0f });
    zox_set(e, AnimateTargetFloat, { 0 });
}

void trigger_canvas_fade_out(ecs *world, entity canvas, float delay, float length) {
    entity e = zox_get_child_by_id(world, canvas, zox_id(CanvasOverlay));
    if (!e) {
        zox_log_error("Failed to find canvas_overlay on canvas");
        return;
    }
    trigger_canvas_overlay_fade_out(world, e, delay, length);
}

void trigger_canvas_fade_in(ecs *world, entity canvas, float delay, float length) {
    entity e = zox_get_child_by_id(world, canvas, zox_id(CanvasOverlay));
    if (!e) {
        zox_log_error("Failed to find canvas_overlay on canvas");
        return;
    }
    clear_animation_sequence(world, e);
    zox_set(e, AnimationState, { zox_animate_alpha });
    zox_set(e, AnimationStart, { zox_current_time });
    zox_set(e, AnimationLength, { length });
    zox_set(e, AnimationDelay, { delay });
    zox_set(e, AnimateSourceFloat, { 0 });
    zox_set(e, AnimateTargetFloat, { 1 });
    zox_set(e, RenderDisabled, { 0 });
}

void trigger_canvas_fade_transition(ecs* world, entity canvas, double fade_time, double delay_time) {
    entity e = zox_get_child_by_id(world, canvas, zox_id(CanvasOverlay));
    if (!e) {
        zox_log_error("Failed to find canvas_overlay on canvas");
        return;
    }
    zox_set(e, Layer, { get_game_overlay_layer() });
    zox_set(e, AnimationIndex, { 0 });
    zox_set(e, AnimationDelay, { 0 });
    zox_set(e, RenderDisabled, { 0 });
    // i should add multiple animationions as children or something
    zox_muter(e, AnimationSequence, animationSequence);
    zox_muter(e, AnimationTimes, animationTimes);
    zox_muter(e, AnimationTargets, animationTargets);
    resize_memory_component(AnimationSequence, animationSequence, byte, 3);
    resize_memory_component(AnimationTimes, animationTimes, double, 3);
    resize_memory_component(AnimationTargets, animationTargets, float, 3);
    animationSequence->value[0] = zox_animate_alpha;
    animationTimes->value[0] = fade_time;
    animationTargets->value[0] = 1;
    animationSequence->value[1] = zox_animation_waiting;
    animationTimes->value[1] = delay_time;
    animationTargets->value[1] = 1;
    animationSequence->value[2] = zox_animate_alpha;
    animationTimes->value[2] = fade_time;
    animationTargets->value[2] = 0;
}

void trigger_canvas_half_fade(
    ecs *world,
    entity canvas,
    float time_length,
    float alpha,
    byte direction)
{
    byte layer = 2;
    float canvas_fade_delay = 0.02f;
    entity overlay = zox_get_child_by_id(world, canvas, zox_id(CanvasOverlay));
    if (!zox_valid(overlay)) {
        zox_loge("Failed to find canvas_overlay on canvas");
        return;
    }
    // zox_log(" + [%lu] triggering half fade for canvas_overlay [%lu]\n", canvas, e)
    zox_set(overlay, Layer, { layer });
    zox_set(overlay, RenderDisabled, { 0 });
    // i should add multiple animationions as children or something
    clear_animation_sequence(world, overlay);
    // set start animation
    zox_set(overlay, AnimationStart, { zox_current_time });
    zox_set(overlay, AnimationState, { zox_animate_alpha });
    zox_set(overlay, AnimationLength, { time_length - canvas_fade_delay });
    zox_set(overlay, AnimationDelay, { canvas_fade_delay });
    if (direction) {
        zox_set(overlay, RenderDisabled, { 0 });
        zox_set(overlay, AnimateSourceFloat, { 0 });
        zox_set(overlay, AnimateTargetFloat, { alpha });
    } else {
        zox_set(overlay, AnimateSourceFloat, { alpha });
        zox_set(overlay, AnimateTargetFloat, { 0 });
    }
}
