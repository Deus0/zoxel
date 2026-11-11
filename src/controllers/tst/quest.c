// tst: quest_tracking, the newest quest a character gets is tracked
//  - gives the character two realm quests the way a give quest dialogue node does
//      (spawn_quest_doing, then track_quest, see zox_event_give_quest)
//  - after each, the character tracks it, the tracker shows it, and the older quest is
//      unlinked from the tracker (so its progress stops changing the tracker's text)
//  - untracking clears the tracker
//  - run: bash bsh/tst.sh quest_tracking
int zox_tst_quest_stage = 0;
int zox_tst_quest_frames = 0;
entity zox_tst_quest_given[2];

// the tracker shows quest's tracker text
static inline byte zox_tst_quest_tracker_shows(ecs* world, entity tracker, entity quest) {
    char* expected = (char*) get_quest_tracker_text(world, quest);
    const byte shows = is_zext(zox_get(tracker, TextData), expected);
    if (expected[0]) {
        free(expected);
    }
    return shows;
}

byte zox_tst_quest_tracking_ready(ecs* world) {
    const entity player = dbg_player;
    const entity character = zox_valid(player) ? zox_get_link(world, player, CharacterLink) : 0;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(character) || !zox_valid(canvas)) {
        zox_tst_waiting = "the player's character";
        return 0;
    }
    zox_tst_quest_frames++;
    const entity tracker = zox_get_child_by_id(world, canvas, zox_id(QuestTracker));
    if (zox_tst_quest_stage == 0) {
        zox_tst_waiting = "playing, with a quest tracker";
        if (zox_getv(player, PlayerState) != zox_player_state_playing || !zox_valid(tracker) || zox_tst_quest_frames < 10) {
            return 0;
        }
    }
    if (zox_tst_quest_frames < 5) {
        return 0;
    }
    // stages: give quest 1, check it, give quest 2, check it (and untrack), check untracked
    const int index = zox_tst_quest_stage / 2;
    if (zox_tst_quest_stage == 0 || zox_tst_quest_stage == 2) {
        // gives the next realm quest, like a give quest node
        const entity realm = zox_get_link(world, character, RealmLink);
        int found = 0;
        entity quest = 0;
        iter it2 = zox_children(world, realm);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count && !quest; j++) {
                if (zox_has(it2.entities[j], Quest) && found++ == index) {
                    quest = it2.entities[j];
                }
            }
        }
        if (!quest) {
            zox_tst_check(0, "realm has fewer than [%i] quests", index + 1);
            return 1;
        }
        zox_tst_quest_given[index] = spawn_quest_doing(world, character, quest);
        track_quest(world, character, zox_tst_quest_given[index]);
        zox_log("[tst] gave quest [%s]", zox_getn(quest));
    } else if (zox_tst_quest_stage == 1 || zox_tst_quest_stage == 3) {
        const entity quest = zox_tst_quest_given[index];
        zox_tst_check(zox_get_link(world, character, QuestTracked) == quest, "quest [%i] isn't tracked after getting it", index + 1);
        zox_tst_check(zox_get_link(world, quest, QuestTrackerLink) == tracker, "quest [%i] isn't linked to the tracker", index + 1);
        zox_tst_check(zox_tst_quest_tracker_shows(world, tracker, quest), "the tracker doesn't show quest [%i]", index + 1);
        if (index == 1) {
            zox_tst_check(!zox_get_link(world, zox_tst_quest_given[0], QuestTrackerLink), "the first quest is still linked to the tracker");
            track_quest(world, character, 0);
        }
    } else {
        zox_tst_check(!zox_get_link(world, character, QuestTracked), "untracking left a quest tracked");
        zox_tst_check(zox_get(tracker, TextData)->length == 0, "untracking didn't clear the tracker");
        return 1;
    }
    zox_tst_quest_stage++;
    zox_tst_quest_frames = 0;
    return 0;
}

void zox_tst_quest_tracking_check(ecs* world) {
    (void) world;
}
