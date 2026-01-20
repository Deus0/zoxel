entity spawn_prefab_process_model(ecs* world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("process_model");
    zox_prefab_set(e, Seed, { 0 });
    zox_prefab_set(e, ModelLink, { 0 });
    // zox_add_tag(e, RunModel);
    // zox_prefab_set(e, DialogueUILink, { 0 });
    // zox_prefab_set(e, SpeakerLinks, { 0 });
    return e;
}