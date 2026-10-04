// NOTE: For now just supports one quest to one marker!
void marker_spawn_system(iter* it) {
    // TODO: Check if has quest to give
    // TODO: Use texture quad instead - simpler
    byte dbg_log = 0;
    color fill = color_yellow;
    color outline = color_black;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_out(ElementLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_o(ElementLinks, elements);
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
        char* text = "!";
        entity marker = spawn_marker(
            world,
            e,
            text,
            fill,
            outline);
        add_to_ElementLinks(elements, marker);
        // link quest to marker
        /*entity quest = zox_get_child_by_id(
            world,
            e,
            zox_id(QuestGiving));*/
        // zox_link(world, quest, MarkerLink, marker);
        // zox_link(world, marker, QuestLink, quest);
        if (dbg_log) {
            zox_log("Spawned Marker Label on [%s]: %s",
                zox_getn(e),
                text);
        }
    }
} zoxd_system(marker_spawn_system);
