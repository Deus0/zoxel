#include "camera2_follow_system.c"

void zoxd_systems_cameras2(ecs *world) {
    zox_system(
        camera2_follow_system,
        zoxp_update,
        [out] transforms3.Position3D,
        [out] transforms3.Rotation3D,
        [none] !cameras.Roaming,
        [none] cameras2.CameraFollower2
    );
}