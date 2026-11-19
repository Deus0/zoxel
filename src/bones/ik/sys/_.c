#include "arm_ik.c"
#include "swing.c"
#include "leg_ik.c"
#include "feet.c"
#include "spine.c"

void zox_systems_ik(ecs* world) {
    // the right arm's guard while raised, a swing (dat/swings.c) as a motion
    zox_system(
        swing_ik_system,
        zoxp_trails,
        [in] bones.SwingStart,
        [in] bones.SwingSpeed,
        [in] bones.RaiseShoulder,
        [in] bones.SwingKind,
        [out] bones.SpineTwist,
        [none] bones.Skeleton
    );
    // the upper body leaning about the hips, the hips shifted (XR's body bending and crouching)
    zox_system(
        spine_lean_system,
        zoxp_trails,
        [in] SpineLean,
        [in] HipsShift,
        [none] bones.Skeleton
    );
    // the upper body turned (a punch's), after its lean: the hips and head turned back
    zox_system(
        spine_twist_system,
        zoxp_trails,
        [in] bones.SpineTwist,
        [out] bones.SpineTwistHead,
        [none] bones.Skeleton
    );
    // feet planted as the body shifts (PlantFeet: XR's body), stepping when far
    zox_system(
        feet_plant_system,
        zoxp_trails,
        [in] rendering3.BoneLinks,
        [none] bones.Skeleton,
        [none] PlantFeet
    );
    // after physics moved the body and the drivers wrote targets (punch zoxp_update, XR hands zoxp_trails), before
    //  the hierarchy places the bones: the weight blends over the animation
    zox_system(
        arm_ik_system,
        zoxp_layouts,
        [in] IKTarget,
        [in] IKWeight,
        [in] transforms3.Position3D,
        [out] transforms3.LocalRotation3D,
        [none] bones.ShoulderBone
    );
    zox_system(
        leg_ik_system,
        zoxp_layouts,
        [in] IKTarget,
        [in] IKWeight,
        [in] transforms3.Position3D,
        [out] transforms3.LocalRotation3D,
        [none] bones.ThighBone
    );
}
