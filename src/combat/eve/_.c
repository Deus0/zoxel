#include "hit.c"

void zox_events_combat(ecs* world) {
    // hit hooks
    zox_on_add(
        hit_camera_shake_event,
        [none] combat.JustHit,
        [none] players.PlayerCharacter,
    );
    // cleaner - keep last, so every hook sees the tag
    zox_on_add(
        hit_cleanup_event,
        [none] combat.JustHit,
    );
}
