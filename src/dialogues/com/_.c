zox_tag(Dialogue);
zox_tag(SpeechNode);
zox_tag(DialogueOptionNode);
zox_tag(DialogueProcess);
zox_tag(DialogueUI);
zox_tag(DialogueLabel);
zox_tag(DialogueButton);
// Dialogue Types
zox_tag(Greetings);
zox_tag(QuestDialogue);
// Move to texts
zoxc_fixed_string(DialogueText, 512);
zox_tag(SpeakerLinks);  // links a dialogue process to its many speakers

void zox_components_dialogues(ecs* world) {
    zoxd_tag(SpeechNode);
    zoxd_tag(DialogueOptionNode);
    zoxd_tag(Dialogue);
    zoxd_tag(DialogueProcess);
    zoxd_nf_link(DialogueUI);
    zoxd_tag(DialogueLabel);
    zoxd_nf_tag(DialogueButton);
    // Dialogue Types
    zoxd_nf_tag(Greetings);
    zoxd_nf_tag(QuestDialogue);
    // Move to texts
    zoxd_fixed_string(DialogueText);
    zoxd_nf_links(SpeakerLinks);
}
