// tst: quest_tracker_text, the tracker's text never shows (null) (it did, now and then)
//  - an unnamed slay quest (no ZoxName, no entity name) of an unnamed character: ??? for both, its count
//  - an invalid quest: an empty text the caller frees (it was a literal "", freed as a tracker spawned)
//  - run: bash bsh/tst.sh quest_tracker_text
byte zox_tst_quest_tracker_text_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_quest_tracker_text_check(ecs* world) {
    const entity character = zox_new();
    const entity quest = zox_new();
    zox_add(quest, Quest);
    zox_add(quest, SlayQuest);
    zox_setv(quest, QuestValue, 1);
    zox_setv(quest, QuestTarget, 3);
    zox_link(world, quest, CharacterLink, character);
    char* text = get_quest_tracker_text(world, quest);
    zox_log("[tst] unnamed slay quest's tracker text [%s]", text ? text : "NULL");
    zox_tst_check(text != NULL, "no text for an unnamed quest");
    if (text) {
        zox_tst_check(!strstr(text, "(null)"), "the tracker shows [%s]", text);
        zox_tst_check(strstr(text, "???") && strstr(text, "[1/3]"), "the tracker shows [%s], not ??? and its count", text);
        free(text);
    }
    char* none = get_quest_tracker_text(world, 0);
    zox_tst_check(none && !none[0], "an invalid quest's text isn't empty");
    free(none);
    zox_delete(quest);
    zox_delete(character);
}
