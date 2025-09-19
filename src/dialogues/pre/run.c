entity spawn_prefab_dialogue_run(
    ecs* world,
    entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("dialogue_run");
    zox_add_tag(e, DialogueRun);
    zox_prefab_set(e, DialogueUILink, { 0 });
    zox_prefab_set(e, SpeakerLinks, { 0 });
    return e;
}