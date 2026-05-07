entity spawn_process_dialogue(ecs* world, entity p, entity node, entity speaker_a, entity speaker_b) {
    zox_instance(p);
    zox_name("dialogue_process");
    zox_set(e, NodeStartLink, { node });
    // Start the run!
    zox_set(e, NodetreeBegin, { zox_dirty_trigger });
    SpeakerLinks speakers = (SpeakerLinks) { 0 };
    speakers.value[0] = speaker_a;
    speakers.value[1] = speaker_b;
    zox_set_ptr(e, SpeakerLinks, speakers);
    return e;
}
