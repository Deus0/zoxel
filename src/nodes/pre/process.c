entity spawn_prefab_process(ecs *world) {
    zox_prefab();
    zox_prefab_name("process");
    zox_add(e, NodeRun);
    zox_add(e, PreInitialize);
    return e;
}
