entity spawn_prefab_zext_input(ecs* world, const entity prefab) {

    zox_prefab_child(prefab);
    zox_prefab_name("zext_input");
    zox_add_tag(e, TextInput);

    return e;
}