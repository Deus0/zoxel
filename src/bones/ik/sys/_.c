#include "arm_ik.c"
#include "punch.c"

void zox_systems_ik(ecs* world) {
    // the right arm's guard while raised, a punch as a motion
    zox_system(
        punch_ik_system,
        zoxp_update,
        [in] bones.SwingStart,
        [in] bones.SwingSpeed,
        [in] bones.RaiseShoulder,
        [none] bones.Skeleton
    );
    // after the drivers and the arm's swing and raise (zoxp_update), so a target lands the frame it's written,
    //  before the hierarchy places the bones: the weight blends over the animation
    zox_system(
        arm_ik_system,
        zoxp_post_update,
        [in] IKTarget,
        [in] IKWeight,
        [in] transforms3.Position3D,
        [out] transforms3.LocalRotation3D,
        [none] bones.ShoulderBone
    );
}
