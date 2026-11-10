entity spawn_process_dialogue(
    ecs* world,
    entity prefab,
    entity node,
    entity speaker_a,
    entity speaker_b)
{
    zox_instance(prefab);
    zox_name("dialogue_process");
    // Start the run!
    zox_link(world, e, NodeLink, node);
    // zox_setv(e, NodetreeBegin, zox_dirty_trigger);
    if (zox_valid(speaker_a) && zox_valid(speaker_b)) {
        zox_link(world, e, SpeakerLinks, speaker_a);
        zox_link(world, e, SpeakerLinks, speaker_b);
    }
    return e;
}
