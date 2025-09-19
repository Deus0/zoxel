entity spawn_dialogue_run(
    ecs* world,
    entity prefab,
    entity tree,
    entity speaker_a,
    entity speaker_b
) {
    zox_instance(prefab);
    zox_name("dialogue_run");
    zox_set(e, NodetreeLink, { tree });
    // Start the run!
    zox_set(e, NodetreeBegin, { zox_dirty_trigger });
    SpeakerLinks speakers = (SpeakerLinks) { 0 };
    speakers.value[0] = speaker_a;
    speakers.value[1] = speaker_b;
    zox_set_ptr(e, SpeakerLinks, speakers);
    return e;
}