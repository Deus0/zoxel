entity spawn_prefab_files(ecs* world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("files");
    zox_add(e, EditorElement);
    return e;
}
