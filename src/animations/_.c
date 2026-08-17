/*
 * +------------------------------------------------------------------+
 * | Zox Module: Animations                                           |
 * |                                                                  |
 * |  Clips - Poses - Frames - Playback - Animation State             |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
#ifndef zoxm_animations
#define zoxm_animations

// TODO: Curve data seperate - curves module
// TODO: Entity for AnimationCurve - Takes Curve, ComponentID, Target Entity, StartTime, EndTime
// TODO: Animation: A list of curves

// Editor: Do Later
// TODO: AnimationViewer - A ui that shows the animation working on a entity tree
// TODO: AnimationEditor - Show list of curves etc like Unity
// TODO: CurveViewer - show a curve - simple for now

// todo: lerp position from A to B entities
// currently we are animating components by hand
// later we must customly choose what curves to apply to what components

#include "dat/animation_states.c"
#include "com/_.c"
// #include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "fun/_.c"

zox_begin_module(Animations) {
    define_components_animations(world);
    define_systems_animations(world);
} zox_end_module(Animations);

#endif
