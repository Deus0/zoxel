// Quest is dirty, update marker
void zox_event_quest_marker(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        // NOTE: Collect quests might switch, if you drop items etc
        set_quest_marker(
            world,
            zox_get_link(world, e, MarkerLink),
            e);
    }
}
