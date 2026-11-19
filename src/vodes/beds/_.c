/*
 * +--------------------------------------------------------------+
 * | Zox Module: Beds                                             |
 * |                                                              |
 * |  Sleep - Lie Down - Eyes Close - Wake                        |
 * |                                                              |
 * |  Uses: Characters - Cameras - Huds - Activations             |
 * |                                                              |
 * +--------------------------------------------------------------+
 *
 *  - a right click on a bed (BlockBed, activations) puts its character to sleep: character_sleep, Sleeping
 *  - asleep (on_sleeping): lying on the bed, its head on the pillow, still (DisableMovement, DisableGravity, NoClip);
 *    a player's eyes close (its camera's CameraLids), its mouse is let go and a window with "Wake" shows
 *  - "Wake" (or character_wake, npcs later) adds Waking: on_waking stands it beside the bed, opens the eyes
 *  - getting on and off is a climb (SleepMotion, sleep_motion_system): up onto the mattress, then down onto its back
 *  - later: skipping the night (day and night), the respawn point, healing while asleep
 */
#include "com/_.c"
#include "fun/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_beds(ecs* world) {
    zox_module(beds);
    zox_components_beds(world);
    zox_on_add(on_sleeping, [none] beds.Sleeping);
    zox_on_add(on_waking, [none] beds.Waking);
    zox_system(
        sleep_motion_system,
        zoxp_physics,
        [in] beds.SleepMotion,
        [out] transforms3.Position3D,
        [out] transforms3.Rotation3D,
        [out] ?physics3.Velocity3D,
    );
#ifdef zox_tests
    zox_tsts_beds(world);
#endif
}
