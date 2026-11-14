// tst: quest_log_tracked, opening the quest log, the tracked quest's icon shows it's tracked (its overlay on)
//  - gives the character the realm's slay quest and tracks it, as a quest giver does, opens the quest log
//  - its icon (DataLink the quest) has its overlay showing (it showed none tracked, only clicking set it)
//  - run: bash bsh/tst.sh quest_log_tracked
byte zox_tst_qlt_stage = 0;
int zox_tst_qlt_frames = 0;
entity zox_tst_qlt_quest = 0;
int zox_tst_qlt_icons = 0, zox_tst_qlt_shown = -1;

// the quest log's icon for quest under e, its overlay shown: -1 no icon, 0 hidden, 1 shown
static inline int zox_tst_qlt_overlay(ecs* world, entity e, entity quest) {
    if (zox_has(e, QuestIcon) && zox_get_link(world, e, DataLink) == quest) {
        zox_tst_qlt_icons++;
        const entity overlay = zox_get_child_by_id(world, e, zox_id(IconOverlay));
        return zox_valid(overlay) && !zox_getv(overlay, RenderDisabled);
    }
    int found = -1;
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count && found < 0; i++) {
            found = zox_tst_qlt_overlay(world, it.entities[i], quest);
        }
    }
    return found;
}

byte zox_tst_quest_log_tracked_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    zox_tst_qlt_frames++;
    zox_tst_progress(zox_tst_qlt_frames);
    zox_tst_waiting = "the quest log";
    if (zox_tst_qlt_stage == 0) {
        const entity realm = zox_get_link(world, character, RealmLink);
        const entity quest = zox_get_child_by_id(world, realm, zox_id(SlayQuest));
        if (!zox_valid(quest)) {
            zox_tst_check(0, "the realm has no slay quest");
            return 1;
        }
        zox_tst_qlt_quest = spawn_quest_doing(world, character, quest);
        track_quest(world, character, zox_tst_qlt_quest);
        zox_tst_qlt_stage = 1;
        zox_tst_qlt_frames = 0;
        return 0;
    }
    if (zox_tst_qlt_stage == 1) {
        if (zox_tst_qlt_frames < 10) {
            return 0;
        }
        toggle_ui_with_id(world, spawn_player_menu_quests, zox_id(MenuQuests), dbg_player);
        zox_tst_qlt_stage = 2;
        zox_tst_qlt_frames = 0;
        return 0;
    }
    if (zox_tst_qlt_frames < 30) {
        return 0;
    }
    const entity canvas = zox_get_link(world, dbg_player, CanvasLink);
    const entity log = zox_get_child_by_id(world, canvas, zox_id(MenuQuests));
    zox_tst_qlt_shown = zox_valid(log) ? zox_tst_qlt_overlay(world, log, zox_tst_qlt_quest) : -1;
    return 1;
}

void zox_tst_quest_log_tracked_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_qlt_shown >= 0, "the quest log has no icon for the tracked quest");
    zox_tst_check(zox_tst_qlt_shown == 1, "the tracked quest's icon doesn't show it's tracked");
}
