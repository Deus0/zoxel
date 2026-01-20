entity spawn_prefab_process(ecs *world) {
    zox_prefab();
    zox_prefab_name("process");
    zox_add_tag(e, NodeRun);
    // per tree
    zox_prefab_set(e, NodeStartLink, { 0 });
    zox_prefab_set(e, NodetreeBegin, { zox_dirty_none });
    zox_prefab_set(e, NodetreeEnd, { zox_dirty_none });
    // per node
    zox_prefab_set(e, NodeLink, { 0 });
    zox_prefab_set(e, NodeBegin, { zox_dirty_none });
    zox_prefab_set(e, NodeEnd, { zox_dirty_none });
    return e;
}