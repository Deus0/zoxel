zox_tag(Dialogue);
zox_tag(DialogueNode);
zox_tag(DialogueRun);
zox_tag(DialogueUI);
zoxc_entity(DialogueUILink);
zoxc_entity(DialogueProcessLink);
zoxc_entity(DialoguetreeLink);
zoxc_entity(DialogueTextLink);
zoxc_fixed_string(DialogueText, 512);
zoxc_fixed_string(TargetText, 512);
zoxc_double(AnimateTextBegin);
zoxc_double(AnimateTextTime);
zoxc_entities(DialoguetreeLinks);
#define zox_speakers_max 2
zoxc_array(SpeakerLinks, entity, zox_speakers_max);

void define_components_dialogues(ecs* world) {
    zoxd_tag(DialogueNode);
    //zoxd_tag(DialogueLeaf);
    zoxd_tag(Dialogue);
    zoxd_tag(DialogueRun);
    zoxd_tag(DialogueUI);
    zoxd_entity(DialogueUILink);
    zoxd_entity(DialogueProcessLink);
    zoxd_entity(DialoguetreeLink);
    zoxd_entity(DialogueTextLink);
    zoxd_fixed_string(DialogueText);
    zoxd_fixed_string(TargetText);
    zoxd_double(AnimateTextBegin);
    zoxd_double(AnimateTextTime);
    zoxd_entities(DialoguetreeLinks);
    zoxd(SpeakerLinks);
}
