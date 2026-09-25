zox_tag(Quest);
zox_tag(QuestDoing);
zox_tag(QuestGiving);
zox_tag(QuestGiver);
zox_tag(QuestDone);
zox_tag(QuestHandedin);
zox_tag(QuestGiven);
zox_tag(SlayQuest);
zox_tag(NodeActionQuest);
zox_tag(NodeGiveQuest);
zox_tag(NodeHandinQuest);
zox_tag(NodeHasQuest);
zox_tag(NodeHasQuestStarted);
zox_tag(NodeHasQuestDone);
zox_tag(NodeHasQuestHandedin);
zoxc_byte(QuestValue);
zoxc_byte(QuestTarget);
// Links
zox_tag(QuestLink);

void zox_components_quests(ecs* world) {
    zoxd_tag(Quest);
    zoxd_tag(QuestDoing);
    zoxd_tag(QuestGiving);
    zoxd_tag(QuestGiver);
    zoxd_tag(QuestGiven);
    zoxd_tag(SlayQuest);
    zoxd_tag(NodeActionQuest);
    zoxd_tag(NodeGiveQuest);
    zoxd_tag(NodeHasQuest);
    zoxd_tag(NodeHandinQuest);
    zoxd_tag(NodeHasQuestStarted);
    zoxd_tag(NodeHasQuestDone);
    zoxd_tag(NodeHasQuestHandedin);
    zoxd_byte(QuestValue);
    zoxd_byte(QuestTarget);
    // Propreties
    zoxd_tag(QuestDone);
    zoxd_tag(QuestHandedin);
    // Links
    zoxd_nf_tag(QuestLink);
}
