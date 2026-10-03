// Quest is dirty, update marker
void zox_event_quest_marker(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity marker = zox_get_link(world, e, MarkerLink);
        if (!marker) {
            continue;
        }
        // NOTE: Collect quests might switch, if you drop items etc
        byte quest_done = zox_has(e, QuestDone);
        const char* text = quest_done ?
            "=" :
            "+";
        // update marker based on quest
        // set to ?
        entity text_entity = zox_get_child_by_id_recursive(
            world,
            marker,
            zox_id(Text));
        if (text_entity) {
            set_entity_text(
                world,
                text_entity,
                text);
        } else {
            zox_loge("Marker text not found");
        }
    }
}
