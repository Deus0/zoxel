#include "allquests.c"
#ifdef zox_tests
#include "quest.c"
#include "quest_log_tracked.c"
#include "tracker_text.c"

void zox_tsts_quests(ecs* world) {
    zox_tsts_quest(world);
    zox_tsts_quest_log_tracked(world);
    zox_tst_add("quest_tracker_text", zox_tst_quest_tracker_text_ready, zox_tst_quest_tracker_text_check, 30);
    zox_tst_set_setup("quest_tracker_text", NULL, 1);
}
#endif
