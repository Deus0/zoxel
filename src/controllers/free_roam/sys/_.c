#include "move.c"
#include "rotate.c"
#include "toggle.c"

void zox_systems_free_roam(ecs* world) {
    zox_system(
        free_camera_move_system,
        zoxp_update,
        [none] players.Player
    );
    zox_system(
        free_camera_rotate_system,
        zoxp_update,
        [none] players.Player
    );
    zox_system(
        free_camera_toggle_system,
        zoxp_update,
        [none] players.Player
    );
}
