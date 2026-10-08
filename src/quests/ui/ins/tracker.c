entity spawn_quest_tracker(
    ecs* world,
    entity canvas,
    entity player)
{
    if (!zox_valid(canvas) ||
        !zox_has(canvas, Canvas))
    {
        zox_loge("Invalid Canvas");
        return 0;
    }
    entity character = zox_get_link(world, player, CharacterLink);
    if (!zox_valid(character)) {
        zox_loge("Cannot spawn quest tracker, no character");
        return 0;
    }
    entity quest = zox_get_link(world, character, QuestTracked);
    char* text = "";
    if (quest) {
        text = (char*) get_quest_tracker_text(world, quest);
    }
    byte font_size = 12;
    byte2 padding = (byte2) {
        12,
        8
    };
    float2 anchor = float2_top_right;
    // position under minimap
    int2 position = (int2) {
        -16,
        -16 - 220
    };
    // convert tracked quest, to label
    entity tracker = spawn_label(
        world,
        prefab_label2t,
        canvas,
        position,
        anchor,
        padding,
        text, // "Quest Tracker",
        font_size,
        zox_alignment_top_right,
        0,
        button_fill,
        button_outline,
        button_font_fill,
        button_font_outline);
    zox_set_unique_name(tracker, "quest_tracker");
    zox_add(tracker, QuestTracker);
    // Link to player character for quests
    // player
    if (quest) {
        entity old_tracker = zox_get_link(world, quest, QuestTrackerLink);
        if (!old_tracker) {
            zox_link(world, quest, QuestTrackerLink, tracker);
        }
        free(text);
    }
    return tracker;
}
