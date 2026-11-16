#include "allquests.c"
#ifdef zox_tests
#include "quest.c"
#include "quest_log_tracked.c"

void zox_tsts_quests(ecs* world) {
    zox_tsts_quest(world);
    zox_tsts_quest_log_tracked(world);
}
#endif
