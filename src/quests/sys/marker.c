// NOTE: For now just supports one quest to one marker!
void marker_spawn_system(iter* it) {
    zox_sys_on_begin();
    // TODO: Check if has quest to give
    // TODO: Use texture quad instead - simpler
    byte dbg_log = 0;
    color outline = color_black;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        if (state->value != zox_dirty_end) {
            continue;
        }
        uint quests = zox_get_children_count_by_id(
            world,
            e,
            zox_id(QuestGiving));
            // zox_id(Quest));
        if (!quests) {
            if (dbg_log) {
                zox_log("No [quests] Marker for [%s]",
                    zox_getn(e));
            }
            continue;
        }
        // seeded npcs respawn with the same quest: show the player's state of it (respawns showed !)
        const entity user_quest = get_players_user_quest(world, e);
        const char* text = get_quest_marker_text(world, user_quest);
        if (!text) {
            continue;   // handed in
        }
        entity marker = spawn_marker(
            world,
            e,
            (char*) text,
            get_quest_marker_fill(world, user_quest),
            outline);
        zox_link(world, e, ElementLinks, marker);
        link_quest_marker(world, user_quest, marker);
        if (dbg_log) {
            zox_log("Spawned Marker Label on [%s]: %s",
                zox_getn(e),
                text);
        }
    }
    zox_sys_on_end();
} zoxd_system(marker_spawn_system);
