zox_tag(Dialogue);
zox_tag(DialogueNode);
zox_tag(DialogueRun);
zox_tag(DialogueUI);
zox_tag(DialogueLabel);
zox_tag(DialogueButton);
zoxc_entity(DialogueUILink);
zoxc_entity(DialogueProcessLink);
zoxc_entity(DialoguetreeLink);
zoxc_entity(DialogueTextLink);
zoxc_fixed_string(DialogueText, 512);
zoxc_entities(DialoguetreeLinks);
#define zox_speakers_max 2
zoxc_array(SpeakerLinks, entity, zox_speakers_max);
// Move to texts
zoxc_fixed_string(TargetText, 512);
zoxc_double(AnimateTextBegin);
zoxc_float(AnimateTextTime);
zoxc_float2(AnimateTextTimeLimits);
zoxc_state(ZigelSpawnedDirty);
zoxc_state(AnimateTextEnded);

void define_components_dialogues(ecs* world) {
    zoxd_tag(DialogueNode);
    zoxd_tag(Dialogue);
    zoxd_tag(DialogueRun);
    zoxd_tag(DialogueUI);
    zoxd_tag(DialogueLabel);
    zoxd_tag(DialogueButton);
    zoxd_entity(DialogueUILink);
    zoxd_entity(DialogueProcessLink);
    zoxd_entity(DialoguetreeLink);
    zoxd_entity(DialogueTextLink);
    zoxd_fixed_string(DialogueText);
    zoxd_entities(DialoguetreeLinks);
    zoxd(SpeakerLinks);
    // Move to texts
    zoxd_fixed_string(TargetText);
    zoxd_double(AnimateTextBegin);
    zoxd_float(AnimateTextTime);
    zoxd_float2(AnimateTextTimeLimits);
    zoxd_state(ZigelSpawnedDirty);
    zoxd_state(AnimateTextEnded);
}
