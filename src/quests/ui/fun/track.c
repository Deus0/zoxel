// Tracks a quest on its character's player's quest tracker (0 tracks none)
//  - untracks the old quest: unlinks it from the tracker (so its updates stop changing the text)
//      and hides its quest icon's overlay
//  - used when clicking a quest icon, and when a quest is given (the newest quest is tracked)
void track_quest(
    ecs* world,
    entity character,
    entity quest)
{
    if (!zox_valid(character)) {
        return;
    }
    const entity player = zox_get_link(world, character, PlayerLink);
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    const entity tracker = zox_valid(canvas) ? zox_get_child_by_id(world, canvas, zox_id(QuestTracker)) : 0;
    const entity old_quest = zox_get_link(world, character, QuestTracked);
    if (zox_valid(old_quest) && old_quest != quest) {
        zox_unlink(world, character, QuestTracked, old_quest);
        const entity old_tracker = zox_get_link(world, old_quest, QuestTrackerLink);
        if (old_tracker) {
            zox_unlink(world, old_quest, QuestTrackerLink, old_tracker);
        }
        const entity old_icon = zox_get_link(world, old_quest, QuestIconLink);
        if (old_icon) {
            zox_unlink(world, old_quest, QuestIconLink, old_icon);
            const entity old_overlay = zox_get_child_by_id(world, old_icon, zox_id(IconOverlay));
            if (old_overlay) {
                zox_setv(old_overlay, RenderDisabled, 1);
            }
        }
    }
    if (zox_valid(quest)) {
        zox_link(world, character, QuestTracked, quest);
        if (zox_valid(tracker)) {
            zox_link(world, quest, QuestTrackerLink, tracker);
        }
        // its text again once it's whole (quest_tracker_updated): tracked as it's given, from a system its spawn is
        //  deferred, no name or slay data yet (the tracker showed (null), find, the wrong quest)
        zox_add(quest, Dirty);
    }
    if (!zox_valid(tracker)) {
        return;
    }
    if (zox_valid(quest)) {
        char* text = (char*) get_quest_tracker_text(world, quest);
        set_entity_text(world, tracker, text);
        if (text[0]) {
            free(text);
        }
    } else {
        set_entity_text(world, tracker, "");
    }
}
