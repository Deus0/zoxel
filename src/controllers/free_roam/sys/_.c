#include "free_camera_move_system.c"
#include "free_camera_rotate_system.c"
#include "free_camera_toggle_system.c"
#include "free_camera_disable_movement_system.c"

void define_systems_free_roam(ecs* world) {
    zox_system(
        FreeCameraMoveSystem,
        EcsOnUpdate,
        [in] inputs.DeviceLinks,
        [in] cameras.CameraLink,
        [none] players.Player
    );
    zox_system(
        FreeCameraRotateSystem,
        EcsOnUpdate,
        [in] inputs.DeviceLinks,
        [in] cameras.CameraLink,
        [none] players.Player
    );
    zox_system(
        FreeCameraToggleSystem,
        EcsOnUpdate,
        [in] inputs.DeviceLinks,
        [in] cameras.CameraLink,
        [in] games.GameLink,
        [none] players.Player
    );
}