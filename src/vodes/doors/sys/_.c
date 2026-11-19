#include "swing.c"

void zox_systems_doors(ecs* world) {
    zox_system(
        door_swing_system,
        zoxp_update,
        [in] doors.DoorOpen,
        [in] doors.DoorRestPosition,
        [in] doors.DoorRestRotation,
        [in] doors.DoorHinge,
        [in] transforms.Scale1,
        [out] doors.DoorSwing,
        [out] transforms3.Position3D,
        [out] transforms3.Rotation3D,
        [out] transforms.TransformMatrix,
    );
}
