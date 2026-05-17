zox_tag(SlayQuest);
zoxc_byte(QuestValue);
zoxc_byte(QuestTarget);
// zoxc_userdata(Quest);
zox_tag(Quest);
zoxc_entity(QuestLink);
zoxc_entitiesw(QuestLinks);
zoxc_state(QuestDirty);
zoxc_state(QuestsDirty);

void define_components_quests(ecs* world) {
    zoxd_tag(SlayQuest);
    zoxd_tag(Quest);
    zoxd_state(QuestDirty);
    zoxd_entity(QuestLink);
    zoxd_entitiesw(QuestLinks);
    zoxd_state(QuestsDirty);
    zoxd_byte(QuestValue);
    zoxd_byte(QuestTarget);
}
