#include "id_window.c"
#include "statbars_hide.c"
#include "window_toggle.c"
#include "player_respawn.c"

void zox_tsts_stats(ecs* world) {
    zox_tsts_id_window(world);
    zox_tsts_statbars_hide(world);
    zox_tsts_window_toggle(world);
    zox_tsts_player_respawn(world);
}
