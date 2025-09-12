
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
        EcsOnUpdate,
        [in] EternalRotation,
        [out] transforms3.Rotation3D
    );
    zox_system(
        ShrinkSystem,
        EcsOnUpdate,
        [in] AnimationState,
        [in] AnimationStart,
        [out] transforms.Scale1D
    );
    zox_system(
        IdleSystem,
        EcsOnUpdate,
        [in] AnimationState,
        [in] AnimationStart,
        [out] transforms.Scale1D
    );
    zox_system(
        AnimationSequenceSystem,
        EcsPostUpdate,
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
        EcsOnUpdate,
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
        EcsOnUpdate,
        [in] FadeOutEvent,
        [in] AnimationStart,
        [out] rendering.Alpha
    );
    zox_system(
        OscillateSystem,
        EcsOnUpdate,
        [in] OscillateStartPosition3D,
        [in] OscillateDeltaPosition3D,
        [out] transforms3.LocalPosition3D,
        [none] OscillatePosition3D
    );

    zox_system(
        LerpToEntitySystem,
        EcsOnUpdate,
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
        EcsOnUpdate,
        [in] AnimationState,
        [in] AnimationPositionStart,
        [in] AnimationPositionEnd,
        [in] AnimationStart,
        [in] AnimationDelay,
        [in] AnimationLength,
        [out] transforms3.Position3D
    );
}