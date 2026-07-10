#include "follow.c"
#include "attach.c"

void define_systems_cameras3(ecs *world) {
    zox_system(
        Camera3FollowSystem,
        zoxp_update,
        [in] cameras.CameraFollowLink,
        [in] transforms3.LocalPosition3D,
        [out] transforms3.Position3D
    );
    zox_system(
        CameraAttachSystem,
        zoxp_update,
        [in] cameras.AttachDirty,
        [in] core.EntityTarget
    );
}
