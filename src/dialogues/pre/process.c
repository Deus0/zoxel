entity spawn_prefab_process_dialogue(ecs* world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("process_dialogue");
    zox_add(e, DialogueProcess);
    // zox_prefab_set(e, DialogueUILink, { 0 });
    zox_prefab_set(e, SpeakerLinks, { 0 });
    return e;
}
