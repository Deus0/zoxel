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
// Floats
zoxc_float(LerpFloatFrom);
zoxc_float(LerpFloatTo);

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
    zoxd_float(LerpFloatFrom);
    zoxd_float(LerpFloatTo);
}
