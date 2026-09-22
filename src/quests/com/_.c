zox_tag(Quest);
zox_tag(QuestGiven);
zox_tag(SlayQuest);
zox_tag(NodeGiveQuest);
zox_tag(NodeHasQuest);
zoxc_byte(QuestValue);
zoxc_byte(QuestTarget);
zoxc_state(QuestDirty);
zoxc_state(QuestsDirty);
// Links
zox_tag(QuestLink);

void zox_components_quests(ecs* world) {
    zoxd_tag(Quest);
    zoxd_tag(QuestGiven);
    zoxd_tag(SlayQuest);
    zoxd_tag(NodeGiveQuest);
    zoxd_tag(NodeHasQuest);
    zoxd_state(QuestDirty);
    zoxd_state(QuestsDirty);
    zoxd_byte(QuestValue);
    zoxd_byte(QuestTarget);
    // Links
    zoxd_nf_tag(QuestLink);
}
