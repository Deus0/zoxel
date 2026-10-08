zox_tag(MenuQuests);
zox_tag(QuestTracker);
zox_tag(QuestIcon);
zox_tag(QuestIconLink);
zox_tag(QuestTrackerLink);
zox_tag(QuestTracked);

void zox_components_quests_ui(ecs* world) {
    zoxd_tag(MenuQuests);
    zoxd_tag(QuestTracker);
    zoxd_tag(QuestIcon);
    zoxd_nf_tag(QuestIconLink);
    zoxd_nf_tag(QuestTrackerLink);
    zoxd_nf_tag(QuestTracked);
}
