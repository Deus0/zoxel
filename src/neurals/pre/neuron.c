entity spawn_prefab_neuron(ecs *world) {
    zox_prefab();
    zox_prefab_name("neuron");
    zox_add(e, Neuron);
    zox_set(e, Signal, { 0 });
    // zox_add(e, ParentLink)
    // this is only used for rendering / debugging
    zox_set(e, Position2, { float2_zero });
    return e;
}

entity spawn_neuron(ecs *world, entity prefab, entity brain, float2 position) {
    zox_instance(prefab)
    zox_name("neuron")
    // zox_set(e, ParentLink, { brain })
    // zox_set(e, Weight, { weight })
    zox_set(e, Position2, { position })
    zox_set_parent(world, e, brain);
    return e;
}
