entity spawn_prefab_process_dialogue(ecs* world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("process_dialogue");
    zox_add(e, DialogueRun);
    zox_prefab_set(e, DialogueUILink, { 0 });
    zox_prefab_set(e, SpeakerLinks, { 0 });
    return e;
}