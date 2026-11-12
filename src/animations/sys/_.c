#include "eternal_rotation_system.c"    // move this to animation module
#include "shrink_system.c"
#include "idle_system.c"
#include "fadeout_system.c"
#include "fadein_system.c"
#include "animation_sequence_system.c"
#include "animate_alpha_system.c"
#include "oscillate_system.c"
#include "lerp_to_entity.c"
#include "position.c"
#include "lerp.c"
#include "shake.c"

void zox_systems_animations(ecs* world) {
    zox_system(
        eternal_rotation_system,
        zoxp_update,
        [in] EternalRotation,
        [out] transforms3.Rotation3D
    );
    // Shrinking?
    zox_system(
        shrink_system,
        zoxp_update,
        [in] AnimationState,
        [in] AnimationStart,
        [out] transforms.Scale1
    );
    zox_system(
        idle_system,
        zoxp_update,
        [in] AnimationState,
        [in] AnimationStart,
        [out] transforms.Scale1,
        [none] animations.Idling
    );
    zox_system(
        animation_sequence_system,
        zoxp_update,
        [in] AnimationSequence,
        [in] AnimationTimes,
        [in] AnimationTargets,
        [in] AnimationDelay,
        [out] AnimationIndex,
        [out] AnimationLength,
        [out] AnimationStart,
        [out] AnimationState
    );
    zox_system(
        animate_alpha_system,
        zoxp_update,
        [in] AnimationState,
        [in] AnimationStart,
        [in] AnimationLength,
        [in] AnimationDelay,
        [in] AnimateSourceFloat,
        [in] AnimateTargetFloat,
        [out] rendering.Alpha
    );
    zox_system(
        fadeout_system,
        zoxp_update,
        [in] FadeOutTime,
        [in] AnimationStart,
        [out] rendering.Alpha
    );
    zox_system(
        oscillate_system,
        zoxp_update,
        [in] OscillateStartPosition3D,
        [in] OscillateDeltaPosition3D,
        [out] transforms3.LocalPosition3D,
        [none] OscillatePosition3D
    );
    zox_system(
        lerp_to_entity_system,
        zoxp_update,
        [in] AnimationState,
        [in] AnimationPositionStart,
        [in] LerpToTarget,
        [in] AnimationStart,
        [in] AnimationDelay,
        [in] AnimationLength,
        [out] transforms3.Position3D
    );
    zox_system(
        lerp_to_position_system,
        zoxp_update,
        [in] AnimationState,
        [in] AnimationPositionStart,
        [in] AnimationPositionEnd,
        [in] AnimationStart,
        [in] AnimationDelay,
        [in] AnimationLength,
        [out] transforms3.Position3D
    );
    zox_system(
        lerp_float_system,
        zoxp_update,
        [in] AnimationTarget,
        [in] AnimationTargetComponent,
        [in] AnimationStartTime,
        [in] AnimationDuration,
        [in] LerpFloatFrom,
        [in] LerpFloatTo
    );
    zox_system(
        shake_position_system,
        zoxp_update,
        [in] AnimationTarget,
        [in] AnimationStartTime,
        [in] AnimationDuration,
        [in] ShakeStrength,
        [out] ShakeOrigin,
        [out] ShakeOffset,
        [none] ShakePosition
    );
}
