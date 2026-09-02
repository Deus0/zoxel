zox_tag(Quest);
zox_tag(QuestGiven);
zox_tag(SlayQuest);
zox_tag(GiveQuestNode);
zoxc_entity(QuestLink);
zoxc_byte(QuestValue);
zoxc_byte(QuestTarget);
zoxc_state(QuestDirty);
zoxc_state(QuestsDirty);

void define_components_quests(ecs* world) {
    zoxd_tag(Quest);
    zoxd_tag(QuestGiven);
    zoxd_tag(SlayQuest);
    zoxd_tag(GiveQuestNode);
    zoxd_state(QuestDirty);
    zoxd_entity(QuestLink);
    zoxd_state(QuestsDirty);
    zoxd_byte(QuestValue);
    zoxd_byte(QuestTarget);
}
