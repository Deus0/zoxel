// zedit: the editor, boots into its windows (eve/), no main menu
#include "eve/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_zoxgame(ecs* world) {
    zox_module(zoxgame);
    game_name = "zedit";
    zox_events_zedit(world);
#ifdef zox_tests
    zox_tsts_zedit(world);
#endif
}
