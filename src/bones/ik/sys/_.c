#include "arm_ik.c"
#include "punch.c"

void zox_systems_ik(ecs* world) {
    // the right arm's guard while raised, a punch as a motion
    zox_system(
        punch_ik_system,
        zoxp_trails,
        [in] bones.SwingStart,
        [in] bones.SwingSpeed,
        [in] bones.RaiseShoulder,
        [none] bones.Skeleton
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
}
