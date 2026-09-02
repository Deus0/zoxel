zox_tag(Dialogue);
zox_tag(DialogueNode);
zox_tag(DialogueProcess);
zox_tag(DialogueUI);
zox_tag(DialogueLabel);
zox_tag(DialogueButton);
// Dialogue Types
zox_tag(Greetings);
zox_tag(QuestDialogue);
// Move to texts
zoxc_fixed_string(DialogueText, 512);
zoxc_fixed_string(TargetText, 512);
zoxc_double(AnimateTextBegin);
zoxc_float(AnimateTextTime);
zoxc_float2(AnimateTextTimeLimits);
zoxc_state(GlyphSpawnedDirty);
zoxc_state(AnimateTextEnded);
// Use Links
#define zox_speakers_max 2
zoxc_array(SpeakerLinks, entity, zox_speakers_max);

void define_components_dialogues(ecs* world) {
    zoxd_nf_tag(DialogueNode);
    zoxd_nf_tag(Dialogue);
    zoxd_nf_tag(DialogueProcess);
    zoxd_nf_tag(DialogueUI);
    zoxd_nf_tag(DialogueLabel);
    zoxd_nf_tag(DialogueButton);
    // Dialogue Types
    zoxd_nf_tag(Greetings);
    zoxd_nf_tag(QuestDialogue);
    // Move to texts
    zoxd_fixed_string(DialogueText);
    zoxd_fixed_string(TargetText);
    zoxd_double(AnimateTextBegin);
    zoxd_float(AnimateTextTime);
    zoxd_float2(AnimateTextTimeLimits);
    zoxd_state(GlyphSpawnedDirty);
    zoxd_state(AnimateTextEnded);
    // Use Links
    zoxd(SpeakerLinks);
}
