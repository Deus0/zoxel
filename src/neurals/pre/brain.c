entity spawn_prefab_brain(ecs *world) {
    zox_prefab();
    zox_prefab_name("brain");
    zox_add(e, Brain);
    zox_prefab_add(e, BrainInputs);
    zox_prefab_add(e, BrainOutputs);
    zox_prefab_add(e, BrainLinks);
    zox_prefab_set(e, BrainDirty, { 0 });
    return e;
}

entity spawn_brain(ecs *world, entity prefab) {
    zox_instance(prefab);
    zox_name("brain");
    return e;
}
