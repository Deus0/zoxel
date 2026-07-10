
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

void define_systems_animations(ecs* world) {
    zox_system(
        EternalRotationSystem,
        zoxp_update,
        [in] EternalRotation,
        [out] transforms3.Rotation3D
    );
    zox_system(
        ShrinkSystem,
        zoxp_update,
        [in] AnimationState,
        [in] AnimationStart,
        [out] transforms.Scale1D
    );
    zox_system(
        IdleSystem,
        zoxp_update,
        [in] AnimationState,
        [in] AnimationStart,
        [out] transforms.Scale1D
    );
    zox_system(
        AnimationSequenceSystem,
        zoxp_update, // EcsPostUpdate,
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
        AnimateAlphaSystem,
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
        FadeoutSystem,
        zoxp_update,
        [in] FadeOutTime,
        [in] AnimationStart,
        [out] rendering.Alpha
    );
    zox_system(
        OscillateSystem,
        zoxp_update,
        [in] OscillateStartPosition3D,
        [in] OscillateDeltaPosition3D,
        [out] transforms3.LocalPosition3D,
        [none] OscillatePosition3D
    );
    zox_system(
        LerpToEntitySystem,
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
        LerpToPositionSystem,
        zoxp_update,
        [in] AnimationState,
        [in] AnimationPositionStart,
        [in] AnimationPositionEnd,
        [in] AnimationStart,
        [in] AnimationDelay,
        [in] AnimationLength,
        [out] transforms3.Position3D
    );
}
