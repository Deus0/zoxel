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
    byte font_size = 12;
    byte2 padding = (byte2) { 12, 8 };
    float2 anchor = float2_top_right;
    // position under minimap
    int2 position = (int2) { -16, -16 - 220 };
    entity e = spawn_label(
        world,
        prefab_label2t,
        canvas,
        position,
        anchor,
        padding,
        "Quest Tracker",
        font_size,
        zox_alignment_top_right,
        0,
        button_fill,
        button_outline,
        button_font_fill,
        button_font_outline);
    zox_set_unique_name(e, "quest_tracker");
    zox_add(e, QuestTracker);
    // Link to player character for quests
    // player
    return e;
}
