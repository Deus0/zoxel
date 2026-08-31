entity spawn_prefab_process(ecs *world) {
    zox_prefab();
    zox_prefab_name("process");
    zox_add(e, NodeRun);
    // per tree
    zox_setv(e, NodeStartLink, 0);
    zox_setv(e, NodetreeBegin, 0);
    zox_setv(e, NodetreeEnd, 0);
    // per node
    zox_setv(e, NodeLink, 0);
    zox_setv(e, NodeBegin, 0);
    zox_setv(e, NodeEnd, 0);
    return e;
}
