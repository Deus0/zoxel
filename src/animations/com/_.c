zox_tag(Idling);
zox_tag(OscillatePosition3D);
zoxc_float3(OscillateStartPosition3D);
zoxc_float3(OscillateDeltaPosition3D);
zoxc_byte(AnimationState);
zoxc_double(AnimationStart);
zoxc_double(AnimationLength);
zoxc_double(AnimationDelay);
zoxc_float4(EternalRotation);
zoxc_float(AnimateSourceFloat);
zoxc_float(AnimateTargetFloat);
zoxc_float(FadeOutTime);
zoxc_float3(AnimationPositionStart);
zoxc_float3(AnimationPositionEnd);
zoxc_entity(LerpToTarget);
// sequencing
zoxc_byte(AnimationIndex);
zoxc_arrayd(AnimationSequence, byte) // a sequence of animation states
zoxc_arrayd(AnimationTimes, double) // a sequence of animation times
zoxc_arrayd(AnimationTargets, float) // a sequence of animation target values
zoxc_arrayd(AnimationLinks, entity)
// Tweening
zoxc_entity(AnimationTarget);
zoxc_entity(AnimationTargetComponent);
zoxc_double(AnimationStartTime);
zoxc_float(AnimationDuration);
zoxc_byte(AnimationEase);       // a tween's curve (zox_ease_*), linear without it
zox_tag(AnimationDeletesTarget); // a tween deletes its target when done, i.e. a window closing
zox_tag(AnimationEnded);          // a tween finished (its event, observe it: zox_on_add), it's deleted the frame after
// Floats
zoxc_float(LerpFloatFrom);
zoxc_float(LerpFloatTo);
// Shakes - offsets a targets LocalPosition3D, restores it after
zox_tag(ShakePosition);
zoxc_float(ShakeStrength);
zoxc_float3(ShakeOrigin);   // targets position without shake
zoxc_float3(ShakeOffset);   // offset applied last frame
zox_tag(ShakeLink);         // target -> its active shake

void zox_components_animations(ecs* world) {
    zoxd_tag(Idling);
    zoxd_tag(OscillatePosition3D);
    zoxd_float3(OscillateStartPosition3D);
    zoxd_float3(OscillateDeltaPosition3D);
    // old
    zoxd_byte(AnimationState);
    zoxd_double(AnimationDelay);
    zoxd_float4(EternalRotation);
    // single anim
    zoxd_double(AnimationStart);
    zoxd_double(AnimationLength);
    zoxd_float(AnimateSourceFloat);
    zoxd_float(AnimateTargetFloat);
    // sequencing
    zoxd_byte(AnimationIndex);
    zoxd_arrayd(AnimationSequence);
    zoxd_arrayd(AnimationTimes);
    zoxd_arrayd(AnimationTargets);
    // defined events
    zoxd_float(FadeOutTime);
    // seperating animations
    zoxd_arrayd(AnimationLinks);
    // lerp to
    zoxd_float3(AnimationPositionStart);
    zoxd_float3(AnimationPositionEnd);
    zoxd_entity(LerpToTarget);
    // Tweening
    zoxd_entity(AnimationTarget);
    zoxd_entity(AnimationTargetComponent);
    zoxd_double(AnimationStartTime);
    zoxd_float(AnimationDuration);
    zoxd_byte(AnimationEase);
    zoxd_tag(AnimationDeletesTarget);
    zoxd_tag(AnimationEnded);
    zoxd_float(LerpFloatFrom);
    zoxd_float(LerpFloatTo);
    // Shakes
    zoxd_tag(ShakePosition);
    zoxd_float(ShakeStrength);
    zoxd_float3(ShakeOrigin);
    zoxd_float3(ShakeOffset);
    zoxd_nf_link(ShakeLink);
}
