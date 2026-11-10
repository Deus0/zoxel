#include "follow.c"
#include "attach.c"

void zox_systems_cameras3(ecs *world) {
    zox_system(
        Camera3FollowSystem,
        zoxp_update,
        [in] transforms3.LocalPosition3D,
        [out] transforms3.Position3D,
        [none] cameras.Camera
    );
    zox_system(
        CameraAttachSystem,
        zoxp_update,
        [in] core.EntityTarget,
        [none] cameras.AttachUpdate,
    );
}
